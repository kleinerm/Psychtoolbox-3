/*
	PsychToolbox3/Source/Common/PsychPortAudio/PsychPortAudio.c
	
	PLATFORMS:	All

	AUTHORS:
	Mario Kleiner   mk      mario.kleiner at tuebingen.mpg.de
	
	HISTORY:
	21.03.07		mk		wrote it.  
	
	DESCRIPTION:
	
	Low level Psychtoolbox sound i/o driver. Useful for audio capture, playback and
	feedback with well controlled timing and low latency. Uses the free software
	PortAudio library, API Version 19 (http://www.portaudio.com), which is not GPL,
	but has a more liberal and GPL compatible MIT style license.
	
	This seems to link statically against libportaudio.a on OS/X. However, it doesn't!
	Due to some restrictions of the OS/X linker we can't link statically against portaudio,
	so we have to have a .dylib version of the library installed in a system library
	search path, both for compiling and using PsychPortAudio. The current universal
	binary version of libportaudio.a for OS/X can be found in the...
	PsychSourceGL/Cohorts/PortAudio/ subfolder, which also contains versions for
	Linux and Windows.
	
*/


#include "PsychPortAudio.h"

#if PSYCH_SYSTEM == PSYCH_OSX
#include "pa_mac_core.h"
#endif

#if PSYCH_SYSTEM == PSYCH_WINDOWS
#include "pa_asio.h"
#endif

#define MAX_SYNOPSIS_STRINGS 50  

//declare variables local to this file.  
static const char *synopsisSYNOPSIS[MAX_SYNOPSIS_STRINGS];

#define kPortAudioPlayBack 1
#define kPortAudioCapture  2
#define kPortAudioFullDuplex 3
#define kPortAudioMonitoring 4

// Maximum number of audio devices we handle:
// This consumes around 150-200 Bytes static memory per potential device, so
// we waste about 15 kb or RAM here, which is acceptable nowadays. However: If a device
// is actually opened, it will consume additional memory ressources in PortAudio,
// the operating systems sound subsystem and kernel, and in the audio hardware device
// driver. It will also consume (potentially limited) audio hardware ressources,
// and most importantly, it will consume cpu time, bus cycles and hw/interrupts for the running
// audio processing threads inside PortAudio, the OS sound system and device drivers!
// --> Too many simultaneously open devices may hurt performance and general timing, even
// if the devices are mostly idle!!
// --> Anybody that manages to even come close to this limit does something horribly wrong and certainly non-portable!
#define MAX_PSYCH_AUDIO_DEVS 100

// Maximum number of audio channels we support per open device:
#define MAX_PSYCH_AUDIO_CHANNELS_PER_DEVICE 256

// Initial size (and increment) of audio buffer list. List will grow by that
// many slots whenever it needs to grow:
#define PSYCH_AUDIO_BUFFERLIST_INCREMENT 1024

// Uncomment this define MUTEX_LOCK_TIME_STATS to enable tracing of
// mutex lock hold times for low-level debugging and tuning:
//#define MUTEX_LOCK_TIME_STATS 1

#if PSYCH_SYSTEM == PSYCH_WINDOWS
	// HANDLE to Win32 event object:
	typedef HANDLE psychpa_conditionvar;
#else
	// Posix condition variable:
	typedef pthread_cond_t psychpa_conditionvar;
#endif

typedef struct PsychPASchedule {
	unsigned int	mode;				// Mode of schedule slot: 0 = Invalid slot, > 0 valid slot, where different bits in the int mean something...
	double			repetitions;		// Number of repetitions for the playloop defined in this slot.
	unsigned int loopStartFrame;		// Start of playloop in frames.
	unsigned int loopEndFrame;			// End of playloop in frames.
	int bufferhandle;					// Handle of the playout buffer to use. Zero is the standard playbuffer as set by 'FillBuffer'. Negative handles
										// may have special meaning in future implementations.
} PsychPASchedule;

// Our device record:
typedef struct PsychPADevice {
	psych_mutex	mutex;			// Mutex lock for the PsychPADevice struct.
	psychpa_conditionvar changeSignal;	// Condition variable or event object for change signalling (see above).
	int		 opmode;			// Mode of operation: Playback, capture or full duplex?
	int		 runMode;			// Runmode: 0 = Stop engine at end of playback, 1 = Keep engine running in hot-standby, ...
	PaStream *stream;			// Pointer to associated portaudio stream.
	PaStreamInfo* streaminfo;   // Pointer to stream info structure, provided by PortAudio.
	PaHostApiTypeId hostAPI;	// Type of host API.
	volatile double	 reqStartTime;		// Requested start time in system time (secs).
	volatile double	 startTime;			// Real start time in system time (secs). Returns real start time after start.
								// The real start time is the time when the first sample hit the speaker in playback or full-duplex mode.
								// Its the time when the first sample was captured in pure capture mode - or when the first sample should
								// be captured in pure capture mode with scheduled start. Whenever playback is active, startTime only
								// refers to the output stage.
	volatile double	 captureStartTime;	// Time when first captured sample entered the sound hardware in system time (secs). This information is
								// redundant in pure capture mode - its identical to startTime. In full duplex mode, this is an estimate of
								// when the first sample was captured, whereas startTime is an estimate of when the first sample was output.
	volatile double reqStopTime;	// Requested stop time in system time (secs). Set to DBL_MAX if none requested.
	volatile double estStopTime;	// Estimated sound offset time after stop of playback.
	volatile double currentTime;	// Current playout time of the last sound sample submitted to the engine. Will be wrong in case of playback abort!
	volatile unsigned int state;			// Current state of the stream: 0=Stopped, 1=Hot Standby, 2=Playing, 3=Aborting playback. Mostly written/updated by paCallback.
	volatile unsigned int reqstate;		// Requested state of the stream, as opposed to current 'state'. Written by main-thread, read & processed by paCallback.
	double	 repeatCount;		// Number of repetitions: -1 = Loop forever, 1 = Once, n = n repetitions.
	float*	 outputbuffer;		// Pointer to float memory buffer with sound output data.
	int		 outputbuffersize;	// Size of output buffer in bytes.
	unsigned int loopStartFrame; // Start of current playloop in frames.
	unsigned int loopEndFrame;  // End of current playloop in frames.
	unsigned int playposition;	// Current playposition in samples since start of playback for current buffer and playloop (not frames, not bytes!)
	unsigned int writeposition; // Current writeposition in samples since start of playback (for incremental filling).
	unsigned int totalplaycount; // Total running count of samples since start of playback, accumulated over all buffers and playloop(not frames, not bytes!)
	float*	 inputbuffer;		// Pointer to float memory buffer with sound input data (captured sound data).
	int		 inputbuffersize;	// Size of input buffer in bytes.
	unsigned int recposition;	// Current record position in samples since start of capture.
	unsigned int readposition;  // Last read-out sample since start of capture.
	unsigned int outchannels;	// Number of output channels.
	unsigned int inchannels;	// Number of input channels.
	unsigned int xruns;			// Number of over-/underflows of input-/output channel for this stream.
	unsigned int paCalls;		// Number of callback invocations.
	unsigned int noTime;		// Number of timestamp malfunction - Should not happen anymore.
	unsigned int batchsize;		// Maximum number of frames requested during callback invokation: Estimate of real buffersize.
	double	 predictedLatency;  // Latency that PortAudio predicts for current callbackinvocation. We will compensate for that when starting audio.
	double   latencyBias;		// A bias value to add to the value that PortAudio reports for total buffer->Speaker latency.
								// This value defaults to zero, but can be set up automatically on OS/X or manually on other OSes to compensate
								// for slight mistakes in PA's estimate.
	PsychPASchedule* schedule;	// Pointer to start of array with playback schedule, or a NULL pointer if none defined.
	volatile unsigned int schedule_size;	// Size of schedule array in slots.
	volatile unsigned int schedule_pos;		// Current position in schedule (in slots).
	unsigned int schedule_writepos;			// Current position in schedule (in slots).
} PsychPADevice;

PsychPADevice audiodevices[MAX_PSYCH_AUDIO_DEVS];
unsigned int  audiodevicecount = 0;
unsigned int  verbosity = 4;
double		  yieldInterval = 0.001;	// How long to wait in calls to PsychYieldIntervalSeconds().
boolean		  uselocking = TRUE;		// Use Mutex locking and signalling code for thread synchronization?
boolean		  lockToCore1 = TRUE;		// Lock all engine threads to run on cpu core 1 on Windows to work around broken TSC sync on multi-cores?

double debugdummy1, debugdummy2;

boolean pa_initialized = FALSE;

// Definition of an audio buffer:
struct PsychPABuffer_Struct {
	unsigned int locked;		// locked: >= 1 = Buffer in use by some active audio device. 0 = Buffer unused.
	float*	 outputbuffer;		// Pointer to float memory buffer with sound output data.
	int		 outputbuffersize;	// Size of output buffer in bytes.
	unsigned int outchannels;	// Number of channels.
};

typedef struct PsychPABuffer_Struct PsychPABuffer;

psych_mutex	bufferListmutex;			// Mutex lock for the audio bufferList.
PsychPABuffer*  bufferList;				// Pointer to start of audio bufferList.
unsigned int	bufferListCount;		// Number of slots allocated in bufferList.

// Scan all schedules of all active and open audio devices to check if
// given audiobuffer is referenced. Invalidate reference, if so:
// The special handle == -1 invalidates all references except the ones to special buffer zero.
boolean PsychPAInvalidateBufferReferences(int handle)
{
	int i, j;
	boolean anylocked = FALSE;

	// Scan all open audio devices:
	for(i = 0; i < MAX_PSYCH_AUDIO_DEVS; i++) {
		// Device open?
		if (audiodevices[i].stream) {
			// Schedule attached?
			if (audiodevices[i].schedule) {
				// Scan it and mark all references to our buffer(s) as invalid:
				for (j = 0; j < audiodevices[i].schedule_size; j++) {
					// Slot active and with relevant bufferhandle?
					if ((audiodevices[i].schedule[j].bufferhandle == handle) || ((audiodevices[i].schedule[j].bufferhandle !=0) && (handle == -1)) ) {
						// Invalidate this reference:
						audiodevices[i].schedule[j].mode = 0;
						audiodevices[i].schedule[j].bufferhandle = 0;
						anylocked = TRUE;
					}
				}
			}
		}
	}
	
	return(anylocked);
}

// Create a new audiobuffer for 'outchannels' audio channels and 'nrFrames' samples
// per channel. Init header, allocate zero-filled memory, enqeue in bufferList.
// Resize/Grow bufferList if neccessary. Return handle to buffer.
int PsychPACreateAudioBuffer(unsigned int outchannels, unsigned int nrFrames)
{
	PsychPABuffer* tmpptr;
	int i, handle;
	
	// Does a bufferList exist?
	if ((bufferListCount <= 0) || (bufferList == NULL)) {
		// First call. Allocate and zero-fill initial bufferList:
		bufferList = (PsychPABuffer*) calloc(PSYCH_AUDIO_BUFFERLIST_INCREMENT, sizeof(PsychPABuffer));
		if (NULL == bufferList) PsychErrorExitMsg(PsychError_outofMemory, "Insufficient free memory for allocating new audio buffers when trying to create internal bufferlist!");

		bufferListCount = PSYCH_AUDIO_BUFFERLIST_INCREMENT;
	} 

	// Search a free slot in bufferList: We start at slot 1, ie., we skip slot 0.
	// This because we don't want to ever return a handle of zero, as zero denotes the
	// special per-audiodevice playback buffer.
	i = 1; while ((i < bufferListCount) && (NULL != bufferList[i].outputbuffer)) i++;

	// Success?
	if ((i >= bufferListCount)) {
		// Nope. Could not find free slot. Need to resize the bufferList with more capacity.
		
		// Need to lock bufferList lock to do this:
		PsychLockMutex(&bufferListmutex);
		
		// Reallocate bufferList: This may relocate the bufferList:
		tmpptr = (PsychPABuffer*) realloc((void*) bufferList, (bufferListCount + PSYCH_AUDIO_BUFFERLIST_INCREMENT) * sizeof(PsychPABuffer) );		
		if (NULL == tmpptr) {
			// Failed! Unlock mutex:
			PsychUnlockMutex(&bufferListmutex);
			
			// Error out. The old allocation and parameters are still valid:
			PsychErrorExitMsg(PsychError_outofMemory, "Insufficient free memory for allocating new audio buffers when trying to grow internal bufferlist!");
		}
		
		// Assign new pointer and size:
		bufferList = tmpptr;
		tmpptr = &(bufferList[i]);
		bufferListCount += PSYCH_AUDIO_BUFFERLIST_INCREMENT;
		
		// Initialize new segment of list to zero:
		memset((void*) tmpptr, 0, PSYCH_AUDIO_BUFFERLIST_INCREMENT * sizeof(PsychPABuffer));

		// Done resizing bufferlist. Unlock mutex:
		PsychUnlockMutex(&bufferListmutex);

		// Ready. 'i' now points to first free slot in new segment of extended bufferList.
	}
	
	// Assign slotid of bufferList slot in handle:
	handle = i;

	// Invalidate all potential stale references to the new 'handle' in all schedules:
	PsychPAInvalidateBufferReferences(handle);

	// Allocate actual data buffer:
	bufferList[handle].outputbuffersize = (int) ((outchannels * nrFrames) * sizeof(float));
	bufferList[handle].outchannels = outchannels;
	
	if (NULL == ( bufferList[handle].outputbuffer = (float*) calloc(1, bufferList[handle].outputbuffersize) )) {
		// Out of memory: Release bufferList header and error out:
		PsychErrorExitMsg(PsychError_outofMemory, "Insufficient free memory for allocating new audio buffer when trying to allocate actual buffer!");
	}
	
	// Ok, we're ready with an empty, silence filled audiobuffer. Return its handle:
	return(handle);
}

// Delte all audio buffers and bufferList itself: Called during shutdown.
void PsychPADeleteAllAudioBuffers(void)
{
	int i;
	
	if (bufferListCount > 0) {
		
		// Lock list:
		PsychLockMutex(&bufferListmutex);

		// Invalidate all referencing slots in all schedules:
		PsychPAInvalidateBufferReferences(-1);
		
		// Free all audio buffers:
		for (i = 0; i < bufferListCount; i++) {
			if (NULL != bufferList[i].outputbuffer) free(bufferList[i].outputbuffer);
		}
		
		// Release memory for bufferheader array itself:
		free(bufferList);
		bufferList = NULL;
		bufferListCount = 0;

		// Unlock list:
		PsychUnlockMutex(&bufferListmutex);
	}
	
	return;
}

PsychPABuffer* PsychPAGetAudioBuffer(int handle)
{
	// Does buffer with given handle exist?
	if ((handle < 0) || (handle >= bufferListCount) || (bufferList[handle].outputbuffer == NULL)) {
		PsychErrorExitMsg(PsychError_user, "Invalid audio bufferhandle provided! The handle doesn't correspond to an existing audiobuffer.");
	}
	
	return( &(bufferList[handle]) );
}

// Scan all schedules of all active and open audio devices to check which
// audiobuffers are active and lock them:
boolean PsychPAUpdateBufferReferences(void)
{
	int i, j;
	boolean anylocked = FALSE;
	
	// First we reset all locked flags of all buffers:
	for (i = 0; i < bufferListCount; i++) bufferList[i].locked = 0;
	
	// Scan all open audio devices:
	for(i = 0; i < MAX_PSYCH_AUDIO_DEVS; i++) {
		// Device open?
		if (audiodevices[i].stream) {
			// Schedule attached and device active?
			if ((audiodevices[i].schedule) && ((audiodevices[i].state > 0) && Pa_IsStreamActive(audiodevices[i].stream))) {
				// Active schedule. Scan it and mark all referenced buffers as locked:
				for (j = 0; j < audiodevices[i].schedule_size; j++) {
					// Slot active and with valid bufferhandle?
					if ((audiodevices[i].schedule[j].mode & 2) && (audiodevices[i].schedule[j].bufferhandle > 0)) {
						// Mark used and active audiobuffer as locked:
						bufferList[ audiodevices[i].schedule[j].bufferhandle ].locked = 1;
						anylocked = TRUE;
					}
				}
			}
		}
	}
	
	return(anylocked);
}

// Delete audiobuffer 'handle' if this is possible. If it isn't possible
// at the moment, 'waitmode' will determine the strategy:
int PsychPADeleteAudioBuffer(int handle, int waitmode)
{
	// Retrieve buffer:
	PsychPABuffer* buffer = PsychPAGetAudioBuffer(handle);
	
	// Make sure all buffer locked flags are up to date:
	PsychPAUpdateBufferReferences();
	
	// Buffer locked?
	if (buffer->locked) {
		// Yes :-( In 'waitmode' zero we fail:
		if (waitmode == 0) return(0);
		
		// In waitmode 1, we retry spin-waiting until buffer available:
		while (buffer->locked) {
			PsychYieldIntervalSeconds(yieldInterval);
			PsychPAUpdateBufferReferences();
		}
	}
	
	// Delete buffer:
	if (NULL != buffer->outputbuffer) free(buffer->outputbuffer);
	memset(buffer, 0, sizeof(PsychPABuffer));
	
	// Success:
	return(1);
}

static void PsychPALockDeviceMutex(PsychPADevice* dev)
{
	#ifdef MUTEX_LOCK_TIME_STATS
	// Compute effective mutex lock hold time. Typical duration on a 2nd Rev. MacBookPro DualCore
	// under typical playback load in the paCallback() is less than 100 microseconds, usually around 50 usecs, so
	// nothing to worry about yet.
	PsychGetAdjustedPrecisionTimerSeconds(&debugdummy1);
	#endif
	
	if (uselocking) {
		PsychLockMutex(&(dev->mutex));
	}
}

static void PsychPAUnlockDeviceMutex(PsychPADevice* dev)
{
	if (uselocking) {
		PsychUnlockMutex(&(dev->mutex));
	}

	#ifdef MUTEX_LOCK_TIME_STATS
	PsychGetAdjustedPrecisionTimerSeconds(&debugdummy2);
	dev->predictedLatency = debugdummy2 - debugdummy1;
	#endif
}

static void PsychPACreateSignal(PsychPADevice* dev)
{
	if (uselocking) {
		// Locking and signalling: We have to wait for a signal, and we
		// enter here with the device mutex held. Waiting is operating system dependent:
		#if PSYCH_SYSTEM == PSYCH_WINDOWS
			// MS-Windows: Create auto-reset event-object:
			dev->changeSignal = CreateEvent(NULL,               // default security attributes
											FALSE,              // auto-reset event
											FALSE,              // initial state is nonsignaled
											NULL				// no object name
											); 
		#else
			// Unices aka Posix: Create condition variable:
			pthread_cond_init(&(dev->changeSignal), NULL);
		#endif
	}
}


static void PsychPADestroySignal(PsychPADevice* dev)
{
	if (uselocking) {
		// Locking and signalling: We have to wait for a signal, and we
		// enter here with the device mutex held. Waiting is operating system dependent:
		#if PSYCH_SYSTEM == PSYCH_WINDOWS
			// MS-Windows: Destroy event-object:
			CloseHandle(dev->changeSignal); 
		#else
			// Unices aka Posix: Destroy condition variable:
			pthread_cond_destroy(&(dev->changeSignal));
		#endif
	}
}

static void PsychPASignalChange(PsychPADevice* dev)
{
	if (uselocking) {
		// Locking and signalling: We have to wait for a signal, and we
		// enter here with the device mutex held. Waiting is operating system dependent:
		#if PSYCH_SYSTEM == PSYCH_WINDOWS
			// MS-Windows: Set our event-object to signalled state to wake up waiting master thread, if any:
			SetEvent(dev->changeSignal);
		#else
			// Unices aka Posix: Signal our condition variable to wake up waiting master thread, if any:
			pthread_cond_signal(&(dev->changeSignal));
		#endif
	}
}

static void PsychPAWaitForChange(PsychPADevice* dev)
{
	if (uselocking) {
		// Locking and signalling: We have to wait for a signal, and we
		// enter here with the device mutex held. Waiting is operating system dependent:
		#if PSYCH_SYSTEM == PSYCH_WINDOWS
			// MS-Windows: Unlock mutex, wait for our event-object to go to signalled
			// state, then reacquire the mutex:
			PsychPAUnlockDeviceMutex(dev);
			WaitForSingleObject(dev->changeSignal, INFINITE);
			PsychPALockDeviceMutex(dev);
		#else
			// Unices aka Posix: Wait on a condition variable to signal. Dropping and reaquiring
			// the lock happens automatically in an atomic manner:
			pthread_cond_wait(&(dev->changeSignal), &(dev->mutex));
		#endif
	}
	else {
		// No locking and signalling: Just yield for a bit, then retry...
		PsychYieldIntervalSeconds(yieldInterval);
	}
}

// Callback function which gets called when a portaudio stream (aka our engine) goes idle for any reason:
// This will reset the device state to "idle/stopped" aka 0, reset pending stop requests and signal
// the master thread if it is waiting for this to happen:
void PAStreamFinishedCallback(void *userData)
{
    PsychPADevice* dev = (PsychPADevice*) userData;

	// Lock device struct:
	PsychPALockDeviceMutex(dev);

	// Reset state to zero aka idle / stopped and reset pending requests:
	dev->reqstate = 255;
	
	// Update "true" state to inactive:
	dev->state = 0;

	// If estimated stop time is still undefined at this point, it won't
	// get computed anymore because the engine is stopped. We choose the
	// last submitted samples playout time as best guess of the real
	// stop time. On a regular stop (where hardware plays out all pending
	// audio buffers) this is probably spot-on. On a fast abort however,
	// this may be a too late estimate if the hardware really stopped
	// immediately and dropped pending audio buffers, but we don't know
	// how much was really played out as this is highly hardware dependent:
	if (dev->estStopTime == 0) dev->estStopTime = dev->currentTime;

	// Unlock device struct:
	PsychPAUnlockDeviceMutex(dev);
	
	// Signal state change:
	PsychPASignalChange(dev);
	
	// Ready.
	return;
}

/* Logger callback function to output PortAudio debug messages at 'verbosity' > 5. */
void PALogger(const char* msg)
{
	if (verbosity > 5) printf("PTB-DEBUG: PortAudio says: %s", msg);
	return;
}


// Called exclusively from paCallback, with device-mutex held.
// Check if a schedule is defined. If not, return repetition, playloop and bufferparameters
// from the device struct, ie., old behaviour. If yes, check if an update of the schedule is
// needed (ie., progressing to the next slot) and do so if needed. Return parameters from
// current slot, or an abort signal if end of schedule reached:
int PsychPAProcessSchedule(PsychPADevice* dev, unsigned int *playposition, float** ret_playoutbuffer, unsigned int* ret_outsbsize, unsigned int* ret_outsboffset, double* ret_repeatCount, psych_uint64* ret_playpositionlimit)
{
	int			  loopStartFrame, loopEndFrame;
	unsigned int  outsbsize, outsboffset;
	unsigned long outchannels = dev->outchannels;
	unsigned int  slotid;
	double		  repeatCount;
	psych_uint64  playpositionlimit;
	
	// NULL-Schedule?
	if (dev->schedule == NULL) {
		// Yes: Assign settings from dev-struct:
		*ret_playoutbuffer = dev->outputbuffer;
		outsbsize = dev->outputbuffersize / sizeof(float);

		// Fetch boundaries of playback loop:
		loopStartFrame = dev->loopStartFrame;
		loopEndFrame = dev->loopEndFrame;
		repeatCount = dev->repeatCount;

		// Revalidate boundaries of playback loop:
		if (loopStartFrame * outchannels >= outsbsize) loopStartFrame = (int) (outsbsize / outchannels) - 1;
		if (loopStartFrame < 0) loopStartFrame = 0;
		if (loopEndFrame * outchannels >= outsbsize) loopEndFrame = (int) (outsbsize / outchannels) - 1;
		if (loopEndFrame < 0) loopEndFrame = 0;
		if (loopEndFrame < loopStartFrame) loopEndFrame = loopStartFrame;
		
		// Remap defined playback loop to "corrected" outsbsize and offset for later copy-op:
		outsbsize = (unsigned int) ((loopEndFrame - loopStartFrame + 1) * outchannels);
		outsboffset = (unsigned int) (loopStartFrame * outchannels);
		
		// Compute playpositionlimit, the upper limit of played out samples from loop duration and repeatCount...
		playpositionlimit = ((psych_uint64) (repeatCount * outsbsize));
		// ...and make sure it ends on integral sample frame boundaries:
		playpositionlimit -= playpositionlimit % outchannels;

		// Check if loop and repetition constraints are still valid:
		if ( !((repeatCount == -1) || (*playposition < playpositionlimit)) || (NULL == *ret_playoutbuffer)) {
			// No. Signal end of playback:
			return(0);
		}
	}
	else {
		// No: Real schedule:
		
		do {
			// Find current slot (with wraparound):
			slotid = dev->schedule_pos % dev->schedule_size;
			
			// Current slot valid and pending?
			if ((dev->schedule[slotid].mode & 2) == 0) {
				// No: End of schedule reached - Signal abort request and that's it:
				return(0);
			}
			
			// Current slot is valid: Assign it:
			
			// First outbuffer size and pointer...
			if (dev->schedule[slotid].bufferhandle <= 0) {
				// Default device playoutbuffer:
				*ret_playoutbuffer = dev->outputbuffer;
				outsbsize = dev->outputbuffersize / sizeof(float);
			}
			else
			{
				// Dynamic buffer: Dereference bufferhandle and fetch buffer data for later use:
				
				// Need to lock bufferList lock to do this:
				PsychLockMutex(&bufferListmutex);

				if (bufferList && (dev->schedule[slotid].bufferhandle < bufferListCount)) {
					// Fetch pointer to actual audio data buffer:
					*ret_playoutbuffer = bufferList[dev->schedule[slotid].bufferhandle].outputbuffer;
					
					// Retrieve buffersize in samples:
					outsbsize = bufferList[dev->schedule[slotid].bufferhandle].outputbuffersize / sizeof(float);
					
					// Another child protection:
					if (outchannels != bufferList[dev->schedule[slotid].bufferhandle].outchannels) {
						*ret_playoutbuffer = NULL;
						outsbsize = 0;						
					}
				}
				else {
					*ret_playoutbuffer = NULL;
					outsbsize = 0;
				}
				
				// Unlock bufferList mutex again:
				PsychUnlockMutex(&bufferListmutex);
			}

			// ... then loop and repeat parameters:
			loopStartFrame = dev->schedule[slotid].loopStartFrame;
			loopEndFrame   = dev->schedule[slotid].loopEndFrame;
			repeatCount    = dev->schedule[slotid].repetitions;
			
			// Revalidate boundaries of playback loop:
			if (loopStartFrame * outchannels >= outsbsize) loopStartFrame = (int) (outsbsize / outchannels) - 1;
			if (loopStartFrame < 0) loopStartFrame = 0;
			if (loopEndFrame * outchannels >= outsbsize) loopEndFrame = (int) (outsbsize / outchannels) - 1;
			if (loopEndFrame < 0) loopEndFrame = 0;
			if (loopEndFrame < loopStartFrame) loopEndFrame = loopStartFrame;
			
			// Remap defined playback loop to "corrected" outsbsize and offset for later copy-op:
			outsbsize = (unsigned int) ((loopEndFrame - loopStartFrame + 1) * outchannels);
			outsboffset = (unsigned int) (loopStartFrame * outchannels);
			
			// Compute playpositionlimit, the upper limit of played out samples from loop duration and repeatCount...
			playpositionlimit = ((psych_uint64) (repeatCount * outsbsize));
			// ...and make sure it ends on integral sample frame boundaries:
			playpositionlimit -= playpositionlimit % outchannels;
			
			// Check if loop and repetition constraints as well as actual audio buffer for this slot are still valid:
			if ( !((repeatCount == -1) || (*playposition < playpositionlimit)) || (NULL == *ret_playoutbuffer)) {
				// Constraints violated. This slot is used up: Reset playposition and advance to next slot:
				*playposition = 0;
				dev->schedule[slotid].mode = 1;
				dev->schedule_pos++;
			}
			else {
				// Constraints ok, break out of do-while slot advance loop:
				break;
			}
		} while(TRUE);
	}
	
	*ret_outsbsize = outsbsize;
	*ret_outsboffset = outsboffset;
	*ret_repeatCount = repeatCount;
	*ret_playpositionlimit = playpositionlimit;

	// Safety check: If playoutbuffer is NULL at this point, then somethings screwed
	// and we request abort of playback:
	if (NULL == *ret_playoutbuffer) return(0);

	// Return 1 exit to signal a valid update:
	return(1);
}

/* paCallback: PortAudo I/O processing callback. 
 *
 * This callback is called by PortAudios playback/capture engine whenever
 * it needs new data for playback or has new data from capture. We are expected
 * to take the inputBuffer's content and store it in our own recording buffers,
 * and push data from our playback buffer queue into the outputBuffer.
 *
 * timeInfo tells us useful timing information, so we can estimate latencies,
 * compensate for them, and so on...
 *
 * This callback is part of a realtime/interrupt/system context so don't do
 * things like calling PortAudio functions, allocating memory, file i/o or
 * other unbounded operations!
 */
static int paCallback( const void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void *userData )
{
	// Assign all variables, especially our dev device structure
	// with info about this stream:
    PsychPADevice* dev = (PsychPADevice*) userData;
    float *out = (float*) outputBuffer;
    float *in = (float*) inputBuffer;
	float *playoutbuffer;
    unsigned long i, silenceframes, committedFrames, max_i;
	unsigned long inchannels, outchannels;
	unsigned int  playposition, outsbsize, insbsize, recposition;
	unsigned int  outsboffset;
	int			  loopStartFrame, loopEndFrame;
	unsigned int reqstate;
	double now, firstsampleonset, onsetDelta, offsetDelta, captureStartTime;
	double repeatCount;	
	psych_uint64 playpositionlimit;
	PaHostApiTypeId hA;
	boolean	stopEngine;

	
	// Device struct attached to stream? If no device struct
	// is attached, we can't continue and tell the engine to abort
	// processing of this stream:
	if (dev == NULL) return(paAbort);
	
	// Query host API: Done without mutex held, as it doesn't change during device lifetime:
	hA=dev->hostAPI;

	// Buffer timestamp computation code:
	//
	// This is executed at each callback iteration to provide PTB timebase onset times for
	// the very first sample in this output buffer.
	//
	// The code is time-critical: To be executed as soon as possible after paCallback invocation,
	// therefore at the beginning of control-flow before trying to lock (and potentially stall at) the device mutex!
	//
	// Executing without the mutex locked is safe: It only modifies/reads local variables, and only
	// reads a few device struct variables which are all guaranteed to remain constant while the
	// engine is running.

#if PSYCH_SYSTEM == PSYCH_WINDOWS
	// Super ugly hack for the most broken system in existence: Force
	// the audio thread to cpu core 1, hope that repeated redundant
	// calls don't create scheduling overhead or excessive other overhead.
	// The explanation for this can be found in Source/Windows/Base/PsychTimeGlue.c
	if (lockToCore1) SetThreadAffinityMask(GetCurrentThread(), 1);
#endif
	
	// Retrieve current system time:
	PsychGetAdjustedPrecisionTimerSeconds(&now);
	
	// FIXME: PortAudio stable sets timeInfo->currentTime == 0 --> Breakage!!!
	// That's why we currently have our own PortAudio version.
	
	if (hA==paCoreAudio || hA==paDirectSound || hA==paMME || hA==paALSA) {
		// On these systems, DAC-time is already returned in the system timebase,
		// at least with our modified version of PortAudio, so a simple
		// query will return the onset time of the first sample. Well,
		// looks as if we need to add the device inherent latency, because
		// this describes everything up to the point where DMA transfer is
		// initiated, but not device inherent latency. This additional latency
		// is added via latencyBias, which is initialized in the Open-Function
		// via a low-level driver query to CoreAudio:
		if (dev->opmode & kPortAudioPlayBack) {
			// Playback enabled: Use DAC time as basis for timing:
			firstsampleonset = (double) timeInfo->outputBufferDacTime + dev->latencyBias;
		}
		else {
			// Recording (only): Use ADC time as basis for timing:
			firstsampleonset = (double) timeInfo->inputBufferAdcTime + dev->latencyBias;
		}
		
		// Compute estimated capturetime in captureStartTime. This is only important in
		// full-duplex mode, redundant in pure half-duplex capture mode:
		captureStartTime = (double) timeInfo->inputBufferAdcTime;
	}
	else {
		// ASIO or unknown. ASIO needs to be checked, which category is correct.
		// Not yet verified how these other audio APIs behave. Play safe
		// and perform timebase remapping: This also needs our special fixed
		// PortAudio version where currentTime actually has a value:
		if (dev->opmode & kPortAudioPlayBack) {
			// Playback enabled: Use DAC time as basis for timing:
			// Assign predicted (remapped to our time system) audio onset time for this buffer:
			firstsampleonset = now + ((double) (timeInfo->outputBufferDacTime - timeInfo->currentTime)) + dev->latencyBias;
		}
		else {
			// Recording (only): Use ADC time as basis for timing:
			// Assign predicted (remapped to our time system) audio onset time for this buffer:
			firstsampleonset = now + ((double) (timeInfo->inputBufferAdcTime - timeInfo->currentTime)) + dev->latencyBias;
		}
		
		// Compute estimated capturetime in captureStartTime. This is only important in
		// full-duplex mode, redundant in pure half-duplex capture mode:
		captureStartTime = now + ((double) (timeInfo->inputBufferAdcTime - timeInfo->currentTime));
	}
	
	if (FALSE) {
		// Debug code to compare our two timebases against each other: On OS/X,
		// luckily both timebases are identical, ie. our UpTime() timebase used
		// everywhere in PTB and the CoreAudio AudioHostClock() are identical.
		psych_uint64 ticks;
		double  tickssec;
		PsychGetPrecisionTimerTicksPerSecond(&tickssec);
		PsychGetPrecisionTimerTicks(&ticks);
		printf("AudioHostClock: %lf   vs. System clock: %lf\n", ((double) ticks) / tickssec, now); 
	}
	
	// End of timestamp computation:
	// If audio capture is active, then captureStartTime contains the running estimate of when the first input sample
	// in inbuffer was hitting the audio input - This will be used in state 1 aka hotstandby when deciding to actually start "real" capture.
	//
	// now contains system time when entering this callback - Time in PTB timebase.
	//
	// Most importantly: firstsampleonset contains the estimated onset time (in PTB timebase) of the first sample that
	// we will store in the outputbuffer during this callback invocation. This timestamp is used in multiple places below,
	// e.g., in hotstandby mode (state == 1) to decide when to start actual playback by switching to state 2 and emitting
	// samples to the outputbuffer. Also used if a specific reqEndTime is selected, ie., a sound offset at a scheduled
	// offset time to compute when to stop. It's also used for checking for skipped buffers and other problems...

	// Acquire device lock: We'll likely hold it until exit from paCallback:
	PsychPALockDeviceMutex(dev);
	
	// Cache requested state:
	reqstate = dev->reqstate;

	// Count total number of calls:
	dev->paCalls++;
	
	// Call number of timestamp failures:
	if (timeInfo->currentTime == 0) dev->noTime++;
	
	// Keep track of maximum number of frames requested:
	if (dev->batchsize < framesPerBuffer) dev->batchsize = framesPerBuffer;
	
	// Keep track of buffer over-/underflows:
	if (statusFlags & (paInputOverflow | paInputUnderflow | paOutputOverflow | paOutputUnderflow)) dev->xruns++;

	// Reset number of already committed sample frames for this buffer fill iteration to zero:
	// This is a running count of how much of the current output buffer has been filled with
	// sound content (ie., not simply zero silence padding frames):
	committedFrames = 0;

	// NULL-out pointer to buffer with sound data to play. It will get initialized later on
	// in PsychPAProcessSchedule():
	playoutbuffer = NULL;

	// Query number of output channels:
	outchannels = (unsigned long) dev->outchannels;

	// Query number of output channels:
	inchannels = (unsigned long) dev->inchannels;
	
	// Query number of repetitions:
	repeatCount = dev->repeatCount;
	
	// Get our current playback position in samples (not frames or bytes!!):
	playposition = dev->playposition;
	recposition = dev->recposition;

	// Compute size of soundbuffers in samples:
	outsbsize = dev->outputbuffersize / sizeof(float);
	insbsize = dev->inputbuffersize / sizeof(float);
	
	// Requested logical playback state is "stopped" or "aborting" ? If so, abort.
	if ((reqstate == 0) || (reqstate == 3)) {		
		// Set estimated stop time to last committed sample time, unless it is already set from
		// a previous callback invocation:
		if (dev->estStopTime == 0) dev->estStopTime = dev->currentTime;

		// Acknowledge request by resetting it:
		dev->reqstate = 255;
		
		// Update "true" state to inactive:
		dev->state = 0;

		// Release mutex here, because dev->runMode never changes below us, and
		// all other ops are on local variables:
		PsychPAUnlockDeviceMutex(dev);

		// Signal state change:
		PsychPASignalChange(dev);
		
		// Prime the outputbuffer with silence, so playback is effectively stopped:
		if (outputBuffer) memset(outputBuffer, 0, framesPerBuffer * outchannels * sizeof(float));

		if (dev->runMode == 0) {
			// Runmode 0: We shall really stop the engine:
			
			// Either paComplete gracefully, playing out pending buffers, or
			// request a hard paAbort if abortion is requested:
			return((reqstate == 0) ? paComplete : paAbort);
		}
		else {
			// Runmode 1: We just "idle", ie., set our state to "idle" and return with
			// no further operations. Future invocations of the callback will also turn
			// into no-ops due to the nominal idle state:
			return(paContinue);
		}
	}

	// Are we in a nominally "idle" / inactive state?
	if (dev->state == 0) {
		// We are effectively idle, but the engine shall keep running. This is usually
		// the case in runMode > 0. Here we "simulate" idle state by simply outputting
		// silence and then returning control to PortAudio:
		
		// Set estimated stop time to last committed sample time, unless it is already set from
		// a previous callback invocation:
		if (dev->estStopTime == 0) dev->estStopTime = dev->currentTime;
		
		// Release mutex here, as memset() only operates on "local" data:
		PsychPAUnlockDeviceMutex(dev);
		
		// Prime the outputbuffer with silence to simulate a stopped audio device:
		if (outputBuffer) memset(outputBuffer, 0, framesPerBuffer * outchannels * sizeof(float));

		// Done:
		return(paContinue);
	}

	// This point is only reached in hot-standby or active playback/capture/feedback modes:

	// Check if all required buffers are there. In monitoring mode, we don't need them.
    if (((dev->opmode & kPortAudioMonitoring) == 0) && (dev->opmode & kPortAudioCapture) && (dev->inputbuffer == NULL)) {
		// Acknowledge request by resetting it:
		dev->reqstate = 255;
		
		// Update "true" state to inactive:
		dev->state = 0;

		PsychPAUnlockDeviceMutex(dev);

		// Signal state change:
		PsychPASignalChange(dev);

		return(paAbort);
	}

	// Are we already playing back and/or capturing real audio data,
	// or are we still on hot-standby? PsychPortAudio tries to start
	// playback/capture of a sound exactly at a requested point in
	// system time (i.e. the first sound sample should hit the speaker
	// as closely as possible to that point in time) by use of the
	// following trick: When the user script executes PsychPortAudio('Start'),
	// our routine immediately starts processing of the portaudio stream,
	// starting up the audio hardwares DACs/ADCs and Portaudios engine.
	// After a short latency, our paCallback() (this routine) gets called
	// by the realtime audio scheduler, requesting or providing audio sample
	// data. In the provided timeInfo - variable, we are provided with the
	// current playback time of the audio device (in seconds since stream start)
	// and an estimate of when our first provided sample will hit the speakers.
	// We convert these timestamps into system time, so we'll know how far the user
	// provided onset deadline is away. If the deadline is far away, so the samples
	// for this callback iteration would hit the speaker too early, we simply return
	// a zero filled outputbuffer --> We output silence. If the deadline is somewhere
	// in the middle of this outputbuffer, we fill the appropriate amount of bufferspace
	// with zeros, then copy in our first real samples into the remaining buffer.
	// After that, we switch to real playback, all future calls will provide PA
	// with real sampledata. Assuming the sound onset estimate provided by PA is
	// correct, this should allow accurate sound onset. It all depends on the
	// latency estimate...
	
	// Hot standby?
	if (dev->state == 1) {
		// Hot standby: Compare running buffer timestamps 'now', 'firstsampleonset' and 'captureStartTime'
		// to requested sound onset time and decide if and when to start playback and capture:
		
		// Store our measured/updated PortAudio + HostAPI + Driver + Hardware latency:
		// We'll update the running estimate until transition from hot standby to active:
		dev->predictedLatency = firstsampleonset - now;

		// Ditto for capture starttime:
		if (dev->opmode & kPortAudioCapture) {
			// Store estimated capturetime in captureStartTime. This is only important in
			// full-duplex mode, redundant in pure half-duplex capture mode:
			dev->captureStartTime = captureStartTime;
		}
		
		// Compute difference between requested onset time and presentation time
		// of the first sample of this callbacks returned buffer:
		onsetDelta = dev->reqStartTime - firstsampleonset;
		
		// Time left until onset and in playback mode?
		if ((onsetDelta > 0) && (dev->opmode & kPortAudioPlayBack) && !(dev->opmode & kPortAudioMonitoring)) {
			// Some time left: A full buffer duration?
			if (onsetDelta >= ((double) framesPerBuffer / (double) dev->streaminfo->sampleRate)) {
				// At least one buffer away...
				
				// Release mutex, as remainder only operates on locals:
				PsychPAUnlockDeviceMutex(dev);
				
				// At least one buffer away. Fill our buffer with zeros, aka silence:
				memset(outputBuffer, 0, framesPerBuffer * outchannels * sizeof(float));
				
				// Ready. Tell engine to continue stream processing, i.e., call us again...
				return(paContinue);
			}
			else {
				// A bit time left, but less than a full buffer. Need to pad the head of
				// this buffer with zeros, aka silence, then fill the rest with real data:
				silenceframes = (unsigned long) (onsetDelta * ((double) dev->streaminfo->sampleRate));

				// Fill in some silence:
				memset(outputBuffer, 0, silenceframes * outchannels * sizeof(float));
				out+= (silenceframes * outchannels);

				// Decrement remaining real audio data count:
				framesPerBuffer-=silenceframes;

				// Advance silenceframes into the buffer:
				committedFrames+=silenceframes;

				// dev->startTime now exactly corresponds to onset of first non-silence sample at dev->reqStartTime.
				// At least if everything works properly.
				dev->startTime = dev->reqStartTime;
				
				// Mark us as running:
				dev->state = 2;
				
				// Signal state change:
				PsychPASignalChange(dev);				
			}
		}
		else {
			// Ooops! We are late! Store real estimated onset in the startTime field and
			// then hurry up! If we are in "capture only" mode, we disregard the 'when'
			// deadline and always start immediately.
			dev->startTime = firstsampleonset;

			// Mark us as running:
			dev->state = 2;
			
			// Signal state change:
			PsychPASignalChange(dev);			
		}
	}
    
	// This code only executes in live monitoring mode - a mode where all
	// sound data is fed back immediately with shortest possible latency
	// from input to output, without any involvement of Matlab/Octave code:
	if (dev->opmode & kPortAudioMonitoring) {
		// Copy input buffer to output buffer:
		memcpy(out, in, framesPerBuffer * outchannels * sizeof(float));
		
		// Store updated positions in device structure:
		dev->playposition = playposition + (framesPerBuffer * outchannels);
		dev->recposition = dev->playposition;

		// Compute output time of last fed back sample from this iteration:
		committedFrames += framesPerBuffer;
		dev->currentTime = firstsampleonset + ((double) committedFrames / (double) dev->streaminfo->sampleRate);

		// Return from callback:
		PsychPAUnlockDeviceMutex(dev);
		return(paContinue);
	}
	
	// This code retrieves and stores captured sound data, if any:
	if (dev->opmode & kPortAudioCapture) {
		// This is the simple case (compared to playback processing).
		// Just copy all available data to our internal buffer:
		for (i=0; (i < framesPerBuffer * inchannels); i++) {
			dev->inputbuffer[recposition % insbsize] = (float) *in++;
			recposition++;
		}
		
		// Store updated recording position in device structure:
		dev->recposition = recposition;
	}
	
	// This code emits actual sound data to the engine:
	if (dev->opmode & kPortAudioPlayBack) {
		// Set stopEngine request to false, ie. continue...
		stopEngine = FALSE;
		
		// Last chance to honor a potential playback abort request. Check once more...
		reqstate = dev->reqstate;

		// Compute time delta between requested sound stop time (sound offset time) and
		// time of next sample in to-be-filled buffer, taking potential committedFrames for
		// zero padding at head of buffer into account:
		offsetDelta = dev->reqStopTime - (firstsampleonset + ((double) committedFrames / (double) dev->streaminfo->sampleRate));

		// Clamp to at most 10 seconds ahead, because that is more than enough even
		// for the largest conceivable hostbuffersizes, and it prevents numeric overflow
		// in the math below when converting to unsigned long ints:
		offsetDelta = (offsetDelta > 10.0) ? 10.0 : offsetDelta;

		// Convert remaining time until requested stop time into sample frames until stop:
		offsetDelta = offsetDelta * (double)(dev->streaminfo->sampleRate);

		// Convert into samples: max_i is the maximum allowable value for 'i'
		// in order to satisfy the dev->reqStopTime:
		max_i  = (unsigned long) (offsetDelta * (double) outchannels);
		max_i -= max_i % outchannels;
		
		// Count of outputted frames in this part of the code:
		i=0;

		// Stoptime already reached or abort request from master thread received? If so, stop the engine:
		if (reqstate == 0 || reqstate == 3 || (offsetDelta <= 0) ) stopEngine = TRUE;

		// Repeat until stopEngine condition, or this callbacks host output buffer is full,
		// or max_i timeout reached for end of processing, or no more valid slots available
		// in current schedule. Assign all relevant parameters from schedule:
		while (!stopEngine && (i < framesPerBuffer * outchannels) && (i < max_i) &&
			   PsychPAProcessSchedule(dev, &playposition, &playoutbuffer, &outsbsize, &outsboffset, &repeatCount, &playpositionlimit)) {
			// Process this slot:

			// Copy requested number of samples for each channel into the output buffer: Take the case of
			// "loop forever" and "loop repeatCount" times into account, as well as stop times:
			for (; (i < framesPerBuffer * outchannels) && (i < max_i) && ((repeatCount == -1) || (playposition < playpositionlimit)); i++) {
				*out++ = playoutbuffer[outsboffset + ( playposition % outsbsize )];
				playposition++;
			}

			// Store updated playposition in device structure:
			dev->playposition = playposition;

			// Abort condition?
			if ((i >= max_i) || !PsychPAProcessSchedule(dev, &playposition, &playoutbuffer, &outsbsize, &outsboffset, &repeatCount, &playpositionlimit)) stopEngine = TRUE;
		}

		// Update total count of emitted valid non-silence sample frames:
		committedFrames += i / outchannels;

		// Compute output time of last outputted sample from this iteration:
		dev->currentTime = firstsampleonset + ((double) committedFrames / (double) dev->streaminfo->sampleRate);

		// Update total count of emitted samples since start of playback:
		dev->totalplaycount+= committedFrames * outchannels;
		
		// End of playback reached due to maximum number of possible output samples reached or abortcondition satisfied?
		if ((i < framesPerBuffer * outchannels) || stopEngine) {
			// Premature stop of buffer filling because abortcondition satisfied: Need to go idle or stop the whole engine:
			
			// We need to zero-fill the remainder of the buffer and tell the engine
			// to finish playback:
			while(i < framesPerBuffer * outchannels) {
				*out++ = 0.0;
				i++;
			}
			
			// Signal that engine is stopped/will stop very soonish:
			dev->state = 0;
			dev->reqstate = 255;

			// Set estimated stop time to last committed sample time:
			if (dev->estStopTime == 0) dev->estStopTime = dev->currentTime;
			
			// Safe to unlock, as dev->runMode never changes below us:
			PsychPAUnlockDeviceMutex(dev);
			
			// Signal state change:
			PsychPASignalChange(dev);
			
			if (dev->runMode == 0) {
				// Either paComplete gracefully, playing out pending buffers, or
				// request a hard paAbort if abortion is requested:
				return((reqstate == 3) ? paAbort : paComplete);
			}
			else {
				// Runmode 1 or greater: Have set our state to "idle" already. Just
				// continue:
				return(paContinue);
			}
		}
	}
		
	// Tell engine to continue stream processing, i.e., call us again...
	PsychPAUnlockDeviceMutex(dev);
    return(paContinue);
}

void PsychPACloseStream(int id)
{
	PaStream* stream = audiodevices[id].stream;
	
	if (stream) {
		// Stop, shutdown and release audio stream:
		Pa_StopStream(stream);

		// Unregister the stream finished callback:
		Pa_SetStreamFinishedCallback(stream, NULL);

		// Close and destroy the stream:
		Pa_CloseStream(stream);
		audiodevices[id].stream = NULL;
		
		// Free associated sound outputbuffer:
		if(audiodevices[id].outputbuffer) {
			free(audiodevices[id].outputbuffer);
			audiodevices[id].outputbuffer = NULL;
			audiodevices[id].outputbuffersize = 0;
		}				

		// Free associated sound inputbuffer:
		if(audiodevices[id].inputbuffer) {
			free(audiodevices[id].inputbuffer);
			audiodevices[id].inputbuffer = NULL;
			audiodevices[id].inputbuffersize = 0;
		}

		// Free associated schedule, if any:
		if(audiodevices[id].schedule) {
			free(audiodevices[id].schedule);
			audiodevices[id].schedule = NULL;
			audiodevices[id].schedule_size = 0;
		}				

		// If we use locking, we need to destroy the per-device mutex:
		if (uselocking && PsychDestroyMutex(&(audiodevices[id].mutex))) printf("PsychPortAudio: CRITICAL! Failed to release Mutex object for pahandle %i! Prepare for trouble!\n", id);

		// If we use locking, this will destroy the associated event variable:
		PsychPADestroySignal(&(audiodevices[id]));

		audiodevicecount--;
	}
	
	return;
}

void InitializeSynopsis(void)
{
	int i=0;
	const char **synopsis = synopsisSYNOPSIS;  //abbreviate the long name
	
	synopsis[i++] = "PsychPortAudio - A sound driver built around the PortAudio sound library:\n";
	synopsis[i++] = "\nGeneral information:\n";
	synopsis[i++] = "version = PsychPortAudio('Version');";
	synopsis[i++] = "oldlevel = PsychPortAudio('Verbosity' [,level]);";
	synopsis[i++] = "count = PsychPortAudio('GetOpenDeviceCount');";
	synopsis[i++] = "devices = PsychPortAudio('GetDevices' [,devicetype]);";
	synopsis[i++] = "\nGeneral settings:\n";
	synopsis[i++] = "[oldyieldInterval, oldMutexEnable, lockToCore1] = PsychPortAudio('EngineTunables' [, yieldInterval] [, MutexEnable] [, lockToCore1]);";
	synopsis[i++] = "oldRunMode = PsychPortAudio('RunMode', pahandle [,runMode]);";
	synopsis[i++] = "\n\nDevice setup and shutdown:\n";
	synopsis[i++] = "pahandle = PsychPortAudio('Open' [, deviceid][, mode][, reqlatencyclass][, freq][, channels][, buffersize][, suggestedLatency][, selectchannels]);";
	synopsis[i++] = "PsychPortAudio('Close' [, pahandle]);";
	synopsis[i++] = "oldbias = PsychPortAudio('LatencyBias', pahandle [,biasSecs]);";
	synopsis[i++] = "[underflow, nextSampleStartIndex, nextSampleETASecs] = PsychPortAudio('FillBuffer', pahandle, bufferdata [, streamingrefill=0);";
	synopsis[i++] =	"bufferhandle = PsychPortAudio('CreateBuffer' [, pahandle], bufferdata);";
	synopsis[i++] =	"PsychPortAudio('DeleteBuffer'[, bufferhandle] [, waitmode]);";
	synopsis[i++] =	"PsychPortAudio('RefillBuffer', pahandle [, bufferhandle=0], bufferdata [, startIndex=0]);";
	synopsis[i++] = "PsychPortAudio('SetLoop', pahandle[, startSample=0][, endSample=max][, UnitIsSeconds=0]);";
	synopsis[i++] = "startTime = PsychPortAudio('Start', pahandle [, repetitions=1] [, when=0] [, waitForStart=0] [, stopTime=inf]);";
	synopsis[i++] = "startTime = PsychPortAudio('RescheduleStart', pahandle, when [, waitForStart=0] [, repetitions] [, stopTime]);";
	synopsis[i++] = "status = PsychPortAudio('GetStatus' pahandle);";
	synopsis[i++] = "[audiodata absrecposition overflow cstarttime] = PsychPortAudio('GetAudioData', pahandle [, amountToAllocateSecs][, minimumAmountToReturnSecs][, maximumAmountToReturnSecs]);";
	synopsis[i++] = "[startTime endPositionSecs xruns estStopTime] = PsychPortAudio('Stop', pahandle [,waitForEndOfPlayback=0] [, blockUntilStopped=1] [, repetitions] [, stopTime]);";
	synopsis[i++] =	"PsychPortAudio('UseSchedule', pahandle, enableSchedule [, maxSize = 128]);";
	synopsis[i++] =	"[success, freeslots] = PsychPortAudio('AddToSchedule', pahandle [, bufferHandle=0][, repetitions=1][, startSample=0][, endSample=max][, UnitIsSeconds=0]);";	

	synopsis[i++] = NULL;  //this tells PsychDisplayScreenSynopsis where to stop
	if (i > MAX_SYNOPSIS_STRINGS) {
		PrintfExit("%s: increase dimension of synopsis[] from %ld to at least %ld and recompile.",__FILE__,(long)MAX_SYNOPSIS_STRINGS,(long)i);
	}
}

PaHostApiIndex PsychPAGetLowestLatencyHostAPI(void)
{
	PaHostApiIndex ai;
	
	#if PSYCH_SYSTEM == PSYCH_OSX
		// CoreAudio or nothing ;-)
		return(Pa_HostApiTypeIdToHostApiIndex(paCoreAudio));
	#endif
	
	#if PSYCH_SYSTEM == PSYCH_LINUX
		// Try ALSA first...
		if (((ai=Pa_HostApiTypeIdToHostApiIndex(paALSA))!=paHostApiNotFound) && (Pa_GetHostApiInfo(ai)->deviceCount > 0)) return(ai);
		// Then JACK...
		if (((ai=Pa_HostApiTypeIdToHostApiIndex(paJACK))!=paHostApiNotFound) && (Pa_GetHostApiInfo(ai)->deviceCount > 0)) return(ai);
		// Then OSS...
		if (((ai=Pa_HostApiTypeIdToHostApiIndex(paOSS))!=paHostApiNotFound) && (Pa_GetHostApiInfo(ai)->deviceCount > 0)) return(ai);
		// then give up!
		printf("PTB-ERROR: Could not find an operational audio subsystem on this Linux machine! Soundcard and driver installed and enabled?!?\n");
		return(paHostApiNotFound);
	#endif
	
	#if PSYCH_SYSTEM == PSYCH_WINDOWS
		// Try ASIO first. It's supposed to be the lowest latency API on soundcards that suppport it.
		if (((ai=Pa_HostApiTypeIdToHostApiIndex(paASIO))!=paHostApiNotFound) && (Pa_GetHostApiInfo(ai)->deviceCount > 0)) return(ai);
		// Then WDM kernel streaming (Win2000, XP, maybe Vista). This is the best working free builtin
		// replacement for the proprietary ASIO interface.
		if (((ai=Pa_HostApiTypeIdToHostApiIndex(paWDMKS))!=paHostApiNotFound) && (Pa_GetHostApiInfo(ai)->deviceCount > 0)) return(ai);
		// Then Vistas new WASAPI, which is supposed to replace WDMKS, but is still early alpha quality in PortAudio:
		if (((ai=Pa_HostApiTypeIdToHostApiIndex(paWASAPI))!=paHostApiNotFound) && (Pa_GetHostApiInfo(ai)->deviceCount > 0)) return(ai);
		// Then DirectSound: Bad, but not a complete disaster if the sound card has DS native drivers: 
		if (((ai=Pa_HostApiTypeIdToHostApiIndex(paDirectSound))!=paHostApiNotFound) && (Pa_GetHostApiInfo(ai)->deviceCount > 0)) return(ai);
		// Then Windows MME, a complete disaster, but better than silence...?!? 
		if (((ai=Pa_HostApiTypeIdToHostApiIndex(paMME))!=paHostApiNotFound) && (Pa_GetHostApiInfo(ai)->deviceCount > 0)) return(ai);
		// then give up!
		printf("PTB-ERROR: Could not find an operational audio subsystem on this Windows machine! Soundcard and driver installed and enabled?!?\n");
		return(paHostApiNotFound);
	#endif
	
	printf("PTB-FATAL-ERROR: Impossible point in code execution reached! (End of PsychPAGetLowestLatencyHostAPI()\n");
	return(paHostApiNotFound);
}

PsychError PSYCHPORTAUDIODisplaySynopsis(void)
{
	int i;
	
	for (i = 0; synopsisSYNOPSIS[i] != NULL; i++)
		printf("%s\n",synopsisSYNOPSIS[i]);
	
	return(PsychError_none);
}

// Module exit function: Stop and close all audio streams, terminate PortAudio...
PsychError PsychPortAudioExit(void)
{
	PaError err;
	int i;
	
	if (pa_initialized) {
		for(i=0; i<MAX_PSYCH_AUDIO_DEVS; i++) {
			// Close i'th stream, if it is open:
			PsychPACloseStream(i);
		}
		audiodevicecount = 0;
		
		// Delete all audio buffers and the bufferlist itself:
		PsychPADeleteAllAudioBuffers();
		
		// Release audiobufferlist mutex lock:
		PsychDestroyMutex(&bufferListmutex);
		
		// Shutdown PortAudio itself:
		err = Pa_Terminate();
		if (err) {
			printf("PTB-FATAL-ERROR: PsychPortAudio: Shutdown of PortAudio subsystem failed. Depending on the quality\n");
			printf("PTB-FATAL-ERROR: of your operating system, this may leave the sound system of your machine dead or confused.\n");
			printf("PTB-FATAL-ERROR: Exit and restart Matlab/Octave. Windows users additionally may want to reboot...\n");
			printf("PTB-FATAL-ERRRO: PortAudio reported the following error: %s\n\n", Pa_GetErrorText(err)); 
		}
		else {
			pa_initialized = FALSE;
		}
	}

	// Detach our callback function for low-level debug output:
	PaUtil_SetDebugPrintFunction(NULL);
	
	return(PsychError_none);
}

void PsychPortAudioInitialize(void)
{
	PaError err;
	int i;
	
	// PortAudio already initialized?
	if (!pa_initialized) {
		// Setup callback function for low-level debug output:
		PaUtil_SetDebugPrintFunction(PALogger);

		if ((err=Pa_Initialize())!=paNoError) {
			printf("PTB-ERROR: Portaudio initialization failed with following port audio error: %s \n", Pa_GetErrorText(err));
			PsychErrorExitMsg(PsychError_system, "Failed to initialize PortAudio subsystem.");
		}
		else {
			if(verbosity>2) printf("PTB-INFO: Using specially modified PortAudio engine, based on offical version: %s\n", Pa_GetVersionText());
		}
		
		for(i=0; i<MAX_PSYCH_AUDIO_DEVS; i++) {
			audiodevices[i].stream = NULL;
		}
		
		audiodevicecount=0;

		// Init audio bufferList to empty and Mutex to unlocked:
		bufferListCount = 0;
		bufferList = NULL;
		PsychInitMutex(&bufferListmutex);

		pa_initialized = TRUE;
	}
}

/* PsychPortAudio('Open') - Open and initialize an audio device via PortAudio.
 */
PsychError PSYCHPORTAUDIOOpen(void) 
{
 	static char useString[] = "pahandle = PsychPortAudio('Open' [, deviceid][, mode][, reqlatencyclass][, freq][, channels][, buffersize][, suggestedLatency][, selectchannels]);";
	//															1			 2		 3					4		5			6			  7					  8
	static char synopsisString[] = 
		"Open a PortAudio audio device and initialize it. Returns a 'pahandle' device handle for the device. "
		"All parameters are optional and have reasonable defaults. 'deviceid' Index to select amongst multiple "
		"logical audio devices supported by PortAudio. Defaults to whatever the systems default sound device is. "
		"Different device id's may select the same physical device, but controlled by a different low-level sound "
		"system. E.g., Windows has about five different sound subsystems. 'mode' Mode of operation. Defaults to "
		"1 == sound playback only. Can be set to 2 == audio capture, or 3 for simultaneous capture and playback of sound. "
		"Note however that mode 3 (full duplex) does not work reliably on all sound hardware. On some hardware this mode "
		"may crash Matlab! There is also a special monitoring mode == 7, which only works for full duplex devices "
		"when using the same number of input- and outputchannels. This mode allows direct feedback of captured sounds "
		"back to the speakers with minimal latency and without involvement of your script at all, however no sound "
		"can be captured during this time and your code mostly doesn't have any control over timing etc. \n"
		"'reqlatencyclass' Allows to select how aggressive PsychPortAudio should be about minimizing sound latency "
		"and getting good deterministic timing, i.e. how to trade off latency vs. system load and playing nicely "
		"with other sound applications on the system. Level 0 means: Don't care about latency, this mode works always "
		"and with all settings, plays nicely with other sound applications. Level 1 (the default) means: Try to get "
		"the lowest latency that is possible under the constraint of reliable playback, freedom of choice for all parameters and "
		"interoperability with other applications. Level 2 means: Take full control over the audio device, even if this "
		"causes other sound applications to fail or shutdown. Level 3 means: As level 2, but request the most aggressive "
		"settings for the given device. Level 4: Same as 3, but fail if device can't meet the strictest requirements. "
		"'freq' Requested playback/capture rate in samples per second (Hz). Defaults to a value that depends on the "
		"requested latency mode. 'channels' Number of audio channels to use, defaults to 2 for stereo. If you perform "
		"simultaneous playback and recording, you can provide a 2 element vector for 'channels', specifying different "
		"numbers of output channels and input channels. The first element in such a vector defines the number of playback "
		"channels, the 2nd element defines capture channels. E.g., [2, 1] would define 2 playback channels (stereo) and 1 "
		"recording channel. See the optional 'selectchannels' argument for selection of physical device channels on multi- "
		"channel cards.\n"
		"'buffersize' "
		"requested size and number of internal audio buffers, smaller numbers mean lower latency but higher system load "
		"and some risk of overloading, which would cause audio dropouts. 'suggestedLatency' optional requested latency in "
		"seconds. PortAudio selects internal operating parameters depending on sampleRate, suggestedLatency and buffersize "
		"as well as device internal properties to optimize for low latency output. Best left alone, only here as manual "
		"override in case all the auto-tuning cleverness fails.\n "
		"'selectchannels' optional matrix with mappings of logical channels to device channels: If you only want to use "
		"a subset of the channels present on your sound card, e.g., only 2 playback channels on a 16 channel soundcard, "
		"then you'd set the 'channels' argument to 2. The 'selectchannels' argument allows you to select, e.g.,  which "
		"two of the 16 channels to use for playback. 'selectchannels' is a one row by 'channels' matrix with mappings "
		"for pure playback or pure capture. For full-duplex mode (playback and capture), 'selectchannels' must be a "
		"2 rows by max(channels) column matrix. row 1 will define playback channel mappings, whereas row 2 will then "
		"define capture channel mappings. In any case, the number in the i'th column will define which physical device "
		"channel will be used for playback or capture of the i'th PsychPortAudio channel (the i'th row of your sound "
		"matrix). Numbering of physical device channels starts with zero! Example: Both, playback and simultaneous "
		"recording are requested and 'channels' equals 2, ie, two playback channels and two capture channels. If you'd "
		"specify 'selectchannels' as [0, 6 ; 12, 14], then playback would happen to device channels zero and six, "
		"sound would be captured from device channels 12 and 14. Please note that channel selection is currently "
		"only supported on MS-Windows with ASIO sound cards. The parameter is silently ignored for non-ASIO operation. ";
	
	static char seeAlsoString[] = "Close GetDeviceSettings ";	 
  	
	int freq, buffersize, latencyclass, mode, deviceid, i, numel;
	int* nrchannels;
	int  mynrchannels[2];
	int  m, n, p;
	double* mychannelmap;
	double suggestedLatency, lowlatency;
	PaDeviceIndex paDevice;
	PaHostApiIndex paHostAPI;
	PaStreamParameters outputParameters;
	PaStreamParameters inputParameters;
	PaDeviceInfo* inputDevInfo, *outputDevInfo, *referenceDevInfo;
	PaStreamFlags sflags;
	PaError err;
	PaStream *stream = NULL;
	
	#if PSYCH_SYSTEM == PSYCH_OSX
		paMacCoreStreamInfo hostapisettings;
	#endif
	
	#if PSYCH_SYSTEM == PSYCH_WINDOWS
		// Additional data structures for setup of logical -> device channel
		// mappings on ASIO multi-channel hardware:
		PaAsioStreamInfo  inhostapisettings;
		PaAsioStreamInfo  outhostapisettings;
		int				  outputmappings[MAX_PSYCH_AUDIO_CHANNELS_PER_DEVICE];
		int				  inputmappings[MAX_PSYCH_AUDIO_CHANNELS_PER_DEVICE];
	#endif
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(8));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	if (audiodevicecount >= MAX_PSYCH_AUDIO_DEVS) PsychErrorExitMsg(PsychError_user, "Maximum number of simultaneously open audio devices reached.");
	 
	freq = 0;
	buffersize = 0;
	latencyclass = 1;
	mode = kPortAudioPlayBack;
	deviceid = -1;
	
	// Make sure PortAudio is online:
	PsychPortAudioInitialize();
	
	// Sanity check: Any hardware found?
	if (Pa_GetDeviceCount() == 0) PsychErrorExitMsg(PsychError_user, "Could not find *any* audio hardware on your system! Either your machine doesn't have audio hardware, or somethings seriously screwed.");
	
	// We default to generic system settings for host api specific settings:
	outputParameters.hostApiSpecificStreamInfo = NULL;
	inputParameters.hostApiSpecificStreamInfo  = NULL;

	// Request optional deviceid:
	PsychCopyInIntegerArg(1, kPsychArgOptional, &deviceid);
	if (deviceid < -1) PsychErrorExitMsg(PsychError_user, "Invalid deviceid provided. Valid values are -1 to maximum number of devices.");

	// Request optional mode of operation:
	PsychCopyInIntegerArg(2, kPsychArgOptional, &mode);
	if (mode < 1 || mode > 7 || mode == 4 || mode == 5 || mode == 6) PsychErrorExitMsg(PsychError_user, "Invalid mode provided. Valid values are 1, 2, 3, or 7.");

	// Request optional latency class:
	PsychCopyInIntegerArg(3, kPsychArgOptional, &latencyclass);
	if (latencyclass < 0 || latencyclass > 4) PsychErrorExitMsg(PsychError_user, "Invalid reqlatencyclass provided. Valid values are 0 to 4.");

	if (deviceid == -1) {
		// Default devices requested:
		if (latencyclass == 0) {
			// High latency mode: Simply pick system default devices:
			outputParameters.device = Pa_GetDefaultOutputDevice(); /* Default output device. */
			inputParameters.device  = Pa_GetDefaultInputDevice(); /* Default input device. */
		}
		else {
			// Low latency mode: Try to find the host API which is supposed to be the fastest on
			// a platform, then pick its default devices:
			paHostAPI = PsychPAGetLowestLatencyHostAPI();
			outputParameters.device = Pa_GetHostApiInfo(paHostAPI)->defaultOutputDevice;
			inputParameters.device  = Pa_GetHostApiInfo(paHostAPI)->defaultInputDevice;
		}
	}
	else {
		// Specific device requested: In valid range?
		if (deviceid >= Pa_GetDeviceCount() || deviceid < 0) {
			PsychErrorExitMsg(PsychError_user, "Invalid deviceid provided. Higher than the number of devices - 1 or lower than zero.");
		}
		
		outputParameters.device = (PaDeviceIndex) deviceid;
		inputParameters.device = (PaDeviceIndex) deviceid;
	}

	// Query properties of selected device(s):
	inputDevInfo  = Pa_GetDeviceInfo(inputParameters.device);
	outputDevInfo = Pa_GetDeviceInfo(outputParameters.device);

	// Select one of them as "reference" info devices: It's properties are used whenever
	// no more specialized info is available. We use the output device (if any) as reference,
	// otherwise fall back to the inputdevice.
	referenceDevInfo = (outputDevInfo) ? outputDevInfo : inputDevInfo;

	// Sanity check: Any hardware found?
	if (referenceDevInfo == NULL) PsychErrorExitMsg(PsychError_user, "Could not find *any* audio hardware on your system - or at least not with the provided deviceid, if any!");

	// Check if current set of selected/available devices is compatible with our playback mode:
	if (((mode & kPortAudioPlayBack) || (mode & kPortAudioMonitoring)) && ((outputDevInfo == NULL) || (outputDevInfo && outputDevInfo->maxOutputChannels <= 0))) {
		PsychErrorExitMsg(PsychError_user, "Audio output requested, but there isn't any audio output device available or you provided a deviceid for something else than an output device!");
	}

	if (((mode & kPortAudioCapture) || (mode & kPortAudioMonitoring)) && ((inputDevInfo == NULL) || (inputDevInfo && inputDevInfo->maxInputChannels <= 0))) {
		PsychErrorExitMsg(PsychError_user, "Audio input requested, but there isn't any audio input device available or you provided a deviceid for something else than an input device!");
	}
	
	// Request optional frequency:
	PsychCopyInIntegerArg(4, kPsychArgOptional, &freq);
	if (freq < 0 || freq > 200000) PsychErrorExitMsg(PsychError_user, "Invalid frequency provided. Valid values are 0 to 200000 Hz.");
	
	// Request optional number of channels:
	numel = 0; nrchannels = NULL;
	PsychAllocInIntegerListArg(5, kPsychArgOptional, &numel, &nrchannels);
	if (numel == 0) {
		// No optional channelcount argument provided: Default to two for playback and recording, unless device is
		// a mono device -- then we default to one:
		mynrchannels[0] = (outputDevInfo && outputDevInfo->maxOutputChannels < 2) ? outputDevInfo->maxOutputChannels : 2;
		mynrchannels[1] = (inputDevInfo && inputDevInfo->maxInputChannels < 2) ? inputDevInfo->maxInputChannels : 2;
	}
	else if (numel == 1) {
		// One argument provided: Set same count for playback and recording:
		if (*nrchannels < 1 || *nrchannels > MAX_PSYCH_AUDIO_CHANNELS_PER_DEVICE) PsychErrorExitMsg(PsychError_user, "Invalid number of channels provided. Valid values are 1 to device maximum.");
		mynrchannels[0] = *nrchannels;
		mynrchannels[1] =  *nrchannels;		
	}
	else if (numel == 2) {
		// Separate counts for playback and recording provided: Set'em up.
		if (nrchannels[0] < 1 || nrchannels[0] > MAX_PSYCH_AUDIO_CHANNELS_PER_DEVICE) PsychErrorExitMsg(PsychError_user, "Invalid number of playback channels provided. Valid values are 1 to device maximum.");
		mynrchannels[0] = nrchannels[0];
		if (nrchannels[1] < 1 || nrchannels[1] > MAX_PSYCH_AUDIO_CHANNELS_PER_DEVICE) PsychErrorExitMsg(PsychError_user, "Invalid number of capture channels provided. Valid values are 1 to device maximum.");
		mynrchannels[1] = nrchannels[1];
	}
	else {
		// More than 2 channel counts provided? Impossible.
		PsychErrorExitMsg(PsychError_user, "You specified a list with more than two 'channels' entries? Can only be max 2 for playback- and capture.");
	}

	// Make sure that number of capture and playback channels is the same for fast monitoring/feedback mode:
	if ((mode & kPortAudioMonitoring) && (mynrchannels[0] != mynrchannels[1])) PsychErrorExitMsg(PsychError_user, "Fast monitoring/feedback mode selected, but number of capture and playback channels differs! They must be the same for this mode!");

	// Request optional buffersize:
	PsychCopyInIntegerArg(6, kPsychArgOptional, &buffersize);
	if (buffersize < 0 || buffersize > 4096) PsychErrorExitMsg(PsychError_user, "Invalid buffersize provided. Valid values are 0 to 4096 samples.");

	// Request optional suggestedLatency:
	suggestedLatency = -1.0;
	PsychCopyInDoubleArg(7, kPsychArgOptional, &suggestedLatency);
	if (suggestedLatency!=-1 && (suggestedLatency < 0.0 || suggestedLatency > 1.0)) PsychErrorExitMsg(PsychError_user, "Invalid suggestedLatency provided. Valid values are 0.0 to 1.0 seconds.");
	
	// Get optional channel map:
	mychannelmap = NULL;
	PsychAllocInDoubleMatArg(8, kPsychArgOptional, &m, &n, &p, &mychannelmap);
	if (mychannelmap) {
		// Channelmapping provided: Sanity check it.
		if (m<1 || m>2 || p!=1 || (n!=((mynrchannels[0] > mynrchannels[1]) ? mynrchannels[0] : mynrchannels[1]))) {
			PsychErrorExitMsg(PsychError_user, "Invalid size of 'selectchannels' matrix argument: Must be a one- or two row by max(channels) column matrix!");
		}
		
		// Basic check ok. Build ASIO host specific mapping structure:
		#if PSYCH_SYSTEM == PSYCH_WINDOWS
			// Check for ASIO: This only works for ASIO host API...
			if (Pa_GetHostApiInfo(referenceDevInfo->hostApi)->type == paASIO) {
				// MS-Windows and connected to an ASIO device. Good. Try to assign channel mapping:
				if (mode & kPortAudioPlayBack) {
					// Playback mappings:
					outputParameters.hostApiSpecificStreamInfo = (PaAsioStreamInfo*) &outhostapisettings;
					outhostapisettings.size = sizeof(PaAsioStreamInfo);
					outhostapisettings.hostApiType = paASIO;
					outhostapisettings.version = 1;
					outhostapisettings.flags = paAsioUseChannelSelectors;
					outhostapisettings.channelSelectors = (int*) &outputmappings[0];
					for (i=0; i<mynrchannels[0]; i++) outputmappings[i] = (int) mychannelmap[i * m];
					if (verbosity > 3) {
						printf("PTB-INFO: Will try to use the following logical channel -> device channel mappings for sound output to audio stream %i :\n", audiodevicecount); 
						for (i=0; i<mynrchannels[0]; i++) printf("%i --> %i : ", i+1, outputmappings[i]);
						printf("\n\n");
					}
				}
				
				if (mode & kPortAudioCapture) {
					// Capture mappings:
					inputParameters.hostApiSpecificStreamInfo = (PaAsioStreamInfo*) &inhostapisettings;
					inhostapisettings.size = sizeof(PaAsioStreamInfo);
					inhostapisettings.hostApiType = paASIO;
					inhostapisettings.version = 1;
					inhostapisettings.flags = paAsioUseChannelSelectors;
					inhostapisettings.channelSelectors = (int*) &inputmappings[0];
					// Index into first row of one-row matrix or 2nd row of two-row matrix:
					for (i=0; i<mynrchannels[1]; i++) inputmappings[i] = (int) mychannelmap[(i * m) + (m-1)];
					if (verbosity > 3) {
						printf("PTB-INFO: Will try to use the following logical channel -> device channel mappings for sound capture from audio stream %i :\n", audiodevicecount); 
						for (i=0; i<mynrchannels[1]; i++) printf("%i --> %i : ", i+1, inputmappings[i]);
						printf("\n\n");
					}
				}
				// Mappings setup up. The PortAudio library will sanity check this further against device constraints...
			}
			else {
				// Non ASIO device: No ASIO support --> No channel mapping support.
				if (verbosity > 2) printf("PTB-WARNING: Provided 'selectchannels' channel mapping is ignored because this is not an ASIO enabled sound device.\n");
			}
		
		#else
			// Non MS-Windows system: No ASIO support --> No channel mapping support.
			if (verbosity > 2) printf("PTB-WARNING: Provided 'selectchannels' channel mapping is ignored because this is not MS-Windows running on an ASIO enabled sound device.\n");
		#endif
	}
	
	// Set channel count:
	outputParameters.channelCount = mynrchannels[0];	// Number of output channels.
	inputParameters.channelCount = mynrchannels[1];		// Number of input channels.
	
	// Fix sample format to float for now...
	outputParameters.sampleFormat = paFloat32;
	inputParameters.sampleFormat  = paFloat32;

	// Setup buffersize:
	if (buffersize == 0) {
		// No specific buffersize requested:
		if (latencyclass < 3) {
			// At levels < 3, the frames per buffer is derived from
			// the requested latency in seconds. Portaudio will never
			// choose a value smaller than 64 frames per buffer or smaller
			// than what the device suggests as minimum safe value.
			buffersize = paFramesPerBufferUnspecified;
		}
		else {
			if (Pa_GetHostApiInfo(referenceDevInfo->hostApi)->type == paCoreAudio) {
				buffersize = 64; // Lowest setting that is safe on a fast MacBook-Pro.
			}
			else {
				// ASIO/ALSA/others: Leave it unspecified to get optimal selection by lower level driver:
				// Esp. on Windows/ASIO and with Linux/ALSA, basically any choice other than paFramesPerBufferUnspecified
				// will just lead to trouble and less optimal results, as the sound subsystems are very good at
				// choosing this parameter optimally if allowed, but have a hard job to cope with any user-enforced choices.
				buffersize = paFramesPerBufferUnspecified;
			}
		}
	}
	
	// Now we have auto-selected buffersize or user provided override...
	
	// Setup samplerate:
	if (freq == 0) {
		// No specific frequency requested:
		if (latencyclass < 3) {
			// At levels < 3, we select the device specific default.
			freq = referenceDevInfo->defaultSampleRate;
		}
		else {
			freq = 96000; // Go really high...
		}
	}
	
	// Now we have auto-selected frequency or user provided override...

	// Set requested latency: In class 0 we choose device recommendation for dropout-free operation, in
	// all higher (lowlat) classes we request zero latency. PortAudio will
	// clamp this request to something safe internally.
	switch (Pa_GetHostApiInfo(referenceDevInfo->hostApi)->type) {
		case paCoreAudio:	// CoreAudio driver will automatically clamp to safe minimum. Around 0.7 msecs.
		case paWDMKS:		// dto. for Windows kernel streaming.
			lowlatency = 0.0;
		break;
		
		case paMME:		// No such a thing as low latency, but easy to kill the machine with too low settings!
			lowlatency = 0.1; // Play safe, request 100 msecs. Otherwise terrible things may happen!
		break;
		
		case paDirectSound:	// DirectSound defaults to 120 msecs, which is way too much! It doesn't accept 0.0 msecs.
			lowlatency = 0.02;	// Choose some half-way safe tradeoff: 20 msecs.
		break;
		
		case paASIO:		
			// ASIO: A value of zero would set safe (and high latency!) defaults. Too small values get
			// clamped to a safe minimum by the driver, so we select a very small positive value, say
			// 1 msec to get lowest possible latency for latencyclass of at least 2. In latency class 1
			// we play a bit safer and go for 5 msecs:
			lowlatency = (latencyclass >= 2) ? 0.001 : 0.005;
		break;

		case paALSA:	
			// For ALSA we choose 10 msecs by default, lowering to 5 msecs if exp. requested. Experience
			// shows that the effective latency will be only a fraction of this, so we are good.
			// Otoh going too low could cause dropouts on the tested MacbookPro 2nd generation...
			// This will need more lab testing and tweaking - and the user can override anyway...
			lowlatency = (latencyclass > 2) ? 0.005 : 0.010;
		break;

		default:			// Not the safest assumption for non-verified Api's, but we'll see...
			lowlatency = 0.0;
	}
	
	if (suggestedLatency == -1.0) {
		// None provided: Choose default based on latency mode:
		outputParameters.suggestedLatency = (latencyclass == 0 && outputDevInfo) ? outputDevInfo->defaultHighOutputLatency : lowlatency;
		inputParameters.suggestedLatency  = (latencyclass == 0 && inputDevInfo) ? inputDevInfo->defaultHighInputLatency : lowlatency;
	}
	else {
		// Override provided: Use it.
		outputParameters.suggestedLatency = suggestedLatency;
		inputParameters.suggestedLatency  = suggestedLatency;
	}

	#if PSYCH_SYSTEM == PSYCH_OSX
	// Apply OS/X CoreAudio specific optimizations:
	if (latencyclass > 1) {
		unsigned long flags = paMacCore_ChangeDeviceParameters;
		if (latencyclass > 3) flags|= paMacCore_FailIfConversionRequired;
		paSetupMacCoreStreamInfo( &hostapisettings, flags);
		outputParameters.hostApiSpecificStreamInfo = (paMacCoreStreamInfo*) &hostapisettings;
		inputParameters.hostApiSpecificStreamInfo = (paMacCoreStreamInfo*) &hostapisettings;
	}
	#endif
	
	// Our stream shall be primed initially with our callbacks data, not just with zeroes.
	// In high latency-mode 0, we request sample clipping and dithering, so sound is more
	// high quality on Windows. In low-latency mode, we safe the computation time for that.
	sflags = paPrimeOutputBuffersUsingStreamCallback;
	sflags = (latencyclass <= 0) ? sflags : (sflags | paClipOff | paDitherOff);

	// Try to create & open stream:
	err = Pa_OpenStream(
            &stream,															/* Return stream pointer here on success. */
            ((mode & kPortAudioCapture) ?  &inputParameters : NULL),			/* Requested input settings, or NULL in pure playback case. */
			((mode & kPortAudioPlayBack) ? &outputParameters : NULL),			/* Requested input settings, or NULL in pure playback case. */
            freq,																/* Requested sampling rate. */
            buffersize,															/* Requested buffer size. */
            sflags,																/* Define special stream property flags. */
            paCallback,															/* Our processing callback. */
            &audiodevices[audiodevicecount]);									/* Our own device info structure */
            
	if(err!=paNoError || stream == NULL) {
			printf("PTB-ERROR: Failed to open audio device %i. PortAudio reports this error: %s \n", audiodevicecount, Pa_GetErrorText(err));
			PsychErrorExitMsg(PsychError_system, "Failed to open PortAudio audio device.");
	}
	
	// Setup our final device structure:
	audiodevices[audiodevicecount].opmode = mode;
	audiodevices[audiodevicecount].runMode = 0;
	audiodevices[audiodevicecount].stream = stream;
	audiodevices[audiodevicecount].streaminfo = Pa_GetStreamInfo(stream);
	audiodevices[audiodevicecount].hostAPI = Pa_GetHostApiInfo(referenceDevInfo->hostApi)->type;
	audiodevices[audiodevicecount].startTime = 0.0;
	audiodevices[audiodevicecount].reqStartTime = 0.0;
	audiodevices[audiodevicecount].reqStopTime = DBL_MAX;
	audiodevices[audiodevicecount].estStopTime = 0;
	audiodevices[audiodevicecount].currentTime = 0;		
	audiodevices[audiodevicecount].state = 0;
	audiodevices[audiodevicecount].reqstate = 255;
	audiodevices[audiodevicecount].repeatCount = 1;
	audiodevices[audiodevicecount].outputbuffer = NULL;
	audiodevices[audiodevicecount].outputbuffersize = 0;
	audiodevices[audiodevicecount].inputbuffer = NULL;
	audiodevices[audiodevicecount].inputbuffersize = 0;
	audiodevices[audiodevicecount].outchannels = mynrchannels[0];
	audiodevices[audiodevicecount].inchannels = mynrchannels[1];
	audiodevices[audiodevicecount].latencyBias = 0.0;
	audiodevices[audiodevicecount].schedule = NULL;
	audiodevices[audiodevicecount].schedule_size = 0;
	audiodevices[audiodevicecount].schedule_pos = 0;
	audiodevices[audiodevicecount].schedule_writepos = 0;

	// If we use locking, we need to initialize the per-device mutex:
	if (uselocking && PsychInitMutex(&(audiodevices[audiodevicecount].mutex))) {
		printf("PsychPortAudio: CRITICAL! Failed to initialize Mutex object for pahandle %i! Prepare for trouble!\n", audiodevicecount);
		PsychErrorExitMsg(PsychError_system, "Audio device mutex creation failed!");
	}
	
	// If we use locking, this will create & init the associated event variable:
	PsychPACreateSignal(&(audiodevices[audiodevicecount]));
	
	// Register the stream finished callback:
	Pa_SetStreamFinishedCallback(audiodevices[audiodevicecount].stream, PAStreamFinishedCallback);

	#if PSYCH_SYSTEM == PSYCH_OSX
		// Query low-level audio driver of the CoreAudio HAL for hardware latency:
		// Hmm, maybe not. Don't know if it is worth the hazzle... This is a constant that
		// doesn't change throughout the lifetime of a machine, and it only makes up for about
		// 0.3 msecs on a typical setup. Maybe if a user really needs that 0.3 msecs extra
		// precision, he should simply download the free HALLab from Apple's Website or Developer
		// tools, run it once, read the displayed constant and set it manually at PortAudio startup.
		
		// PaMacCoreStream* corestream = (PaMacCoreStream*) audiodevices[audiodevicecount].stream;
		// audiodevices[audiodevicecount].latencyBias = 
	#endif
	
	if (verbosity > 3) {
		printf("PTB-INFO: New audio device with handle %i opened as PortAudio stream:\n",audiodevicecount);

		if (audiodevices[audiodevicecount].opmode & kPortAudioPlayBack) {
			printf("PTB-INFO: For %i channels Playback: Audio subsystem is %s, Audio device name is ", audiodevices[audiodevicecount].outchannels, Pa_GetHostApiInfo(outputDevInfo->hostApi)->name);
			printf("%s\n", outputDevInfo->name);
		}

		if (audiodevices[audiodevicecount].opmode & kPortAudioCapture) {
			printf("PTB-INFO: For %i channels Capture: Audio subsystem is %s, Audio device name is ", audiodevices[audiodevicecount].inchannels, Pa_GetHostApiInfo(inputDevInfo->hostApi)->name);
			printf("%s\n", inputDevInfo->name);
		}
		
		printf("PTB-INFO: Real samplerate %f Hz. Input latency %f msecs, Output latency %f msecs.\n",
				audiodevices[audiodevicecount].streaminfo->sampleRate, audiodevices[audiodevicecount].streaminfo->inputLatency * 1000.0,
				audiodevices[audiodevicecount].streaminfo->outputLatency * 1000.0);
	}

	// Return device handle:
	PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) audiodevicecount);
	
	// One more audio device...
	audiodevicecount++;

    return(PsychError_none);	
}

/* PsychPortAudio('Close') - Close an audio device via PortAudio.
 */
PsychError PSYCHPORTAUDIOClose(void) 
{
 	static char useString[] = "PsychPortAudio('Close' [, pahandle]);";
	static char synopsisString[] = 
		"Close a PortAudio audio device. The optional 'pahandle' is the handle of the device to close. If pahandle "
		"is omitted, all audio devices will be closed and the driver will shut down.\n";
	static char seeAlsoString[] = "Open GetDeviceSettings ";	 
  	
	int pahandle= -1;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(1));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(0));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgOptional, &pahandle);
	if (pahandle == -1) {
		// Full shutdown requested:
		PsychPortAudioExit();
	}
	else {
		// Close one device:
		if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");
		PsychPACloseStream(pahandle);
		
		// All devices down? Shutdown PortAudio if so:
		if (audiodevicecount == 0) PsychPortAudioExit();
	}
	
	return(PsychError_none);
}

/* PsychPortAudio('FillBuffer') - Fill audio outputbuffer of a device with data.
 */
PsychError PSYCHPORTAUDIOFillAudioBuffer(void) 
{
 	static char useString[] = "[underflow, nextSampleStartIndex, nextSampleETASecs] = PsychPortAudio('FillBuffer', pahandle, bufferdata [, streamingrefill=0);";
	static char synopsisString[] = 
		"Fill audio data playback buffer of a PortAudio audio device. 'pahandle' is the handle of the device "
		"whose buffer is to be filled. 'bufferdata' is usually a Matlab double matrix with audio data in double format. Each "
		"row of the matrix specifies one sound channel, each column one sample for each channel. Only floating point "
		"values in double precision are supported. Samples need to be in range -1.0 to +1.0, 0.0 for silence. This is "
		"intentionally a very restricted interface. For lowest latency and best timing we want you to provide audio "
		"data exactly at the optimal format and sample rate, so the driver can safe computation time and latency for "
		"expensive sample rate conversion, sample format conversion, and bounds checking/clipping.\n"
		"Instead of a matrix, you can also pass in the bufferhandle of an audio buffer as 'bufferdata'. This buffer "
		"must have been created beforehand via PsychPortAudio('CreateBuffer', ...). Its content must satisfy the "
		"same constraints as in case of passing a Matlab matrix. The content will be copied from the given buffer "
		"to the standard audio buffer, so it is safe to delete that source buffer if you want.\n"
		"'streamingrefill' optional: If set to 1, ask the driver to refill the buffer immediately while playback "
		"is active. You can think of this as appending the audio data to the audio data already present in the buffer. "
		"This is useful for streaming playback or for creating live audio feedback loops. However, the current implementation "
		"doesn't really append the audio data. Instead it replaces already played audio data with your new data. This means "
		"that if you try to refill more than what has been actually played, this function will wait until enough storage space "
		"is available. It will also fail if you try to refill more than the total buffer capacity. Default is to not do "
		"streaming refills, i.e., the buffer is filled in one batch while playback is stopped. Such a refill will also "
		"reset any playloop setting done via the 'SetLoop' subfunction to the full size of the refilled buffer.\n"
		"Optionally the function returns the following values:\n"
		"'underflow' A flag: If 1 then the audio buffer underflowed because you didn't refill it in time, ie., some audible "
		"glitches were present in playback and your further playback timing is screwed.\n"
		"'nextSampleStartIndex' This is the absolute index in samples since start of playback of the sample that would "
		"follow after the last sample you added during this 'FillBuffer' call, ie., the first sample during a successive "
		"'FillBuffer' call.\n"
		"'nextSampleETASecs' This value is undefined (NaN) if playback isn't running. During a streaming refill, it contains "
		"the predicted audio onset time in seconds of the sample with index 'nextSampleStartIndex'. Please note that this "
		"prediction can accumulate a prediction error if your buffer is so large that it contains samples that will only "
		"playback far in the future.\n";

	static char seeAlsoString[] = "Open GetDeviceSettings ";	 
  	
	PsychPABuffer* inbuffer;
	int inbufferhandle = 0;
	float*  indatafloat = NULL;

	int inchannels, insamples, p, buffersize;
	unsigned int totalplaycount;
	double*	indata = NULL;
	float*  outdata = NULL;
	int pahandle   = -1;
	int streamingrefill = 0;
	int underrun = 0;
	double currentTime, etaSecs;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(3));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(2)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(3));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");
	if ((audiodevices[pahandle].opmode & kPortAudioPlayBack) == 0) PsychErrorExitMsg(PsychError_user, "Audio device has not been opened for audio playback, so this call doesn't make sense.");

	// Bufferhandle instead of input data matrix provided?
	if (PsychCopyInIntegerArg(2, kPsychArgAnything, &inbufferhandle) && (inbufferhandle > 0)) {
		// Seems so. Double check:
		inbuffer = PsychPAGetAudioBuffer(inbufferhandle);
		
		// Assign properties:
		inchannels = inbuffer->outchannels;
		insamples = inbuffer->outputbuffersize / sizeof(float) / inchannels;
		p = 1;
		indatafloat = inbuffer->outputbuffer;
	}
	else {
		// Regular double matrix with sound data from runtime:
		PsychAllocInDoubleMatArg(2, kPsychArgRequired, &inchannels, &insamples, &p, &indata);
	}

	if (inchannels != audiodevices[pahandle].outchannels) {
		printf("PTB-ERROR: Audio device %i has %i output channels, but provided matrix has non-matching number of %i rows.\n", pahandle, audiodevices[pahandle].outchannels, inchannels);
		PsychErrorExitMsg(PsychError_user, "Number of rows of audio data matrix doesn't match number of output channels of selected audio device.\n");
	}
	
	if (insamples < 1) PsychErrorExitMsg(PsychError_user, "You must provide at least 1 sample in your audio buffer!");
	if (p!=1) PsychErrorExitMsg(PsychError_user, "Audio data matrix must be a 2D matrix, but this one is not a 2D matrix!");
	
	// Get optional streaming refill flag:
	PsychCopyInIntegerArg(3, kPsychArgOptional, &streamingrefill);
	
	// Full refill or streaming refill?
	if (streamingrefill <= 0) {
		// Standard refill with possible buffer reallocation. Engine needs to be
		// stopped, full reset of engine at refill:

		// Wait for playback on this stream to finish, before refilling it:
		PsychPALockDeviceMutex(&audiodevices[pahandle]);
		while (audiodevices[pahandle].state > 0) {
			// Wait for a state-change before reevaluating the .state:
			PsychPAWaitForChange(&audiodevices[pahandle]);
		}
		
		// Device is idle, we hold the lock. We can safely drop the lock here and still modify
		// device data, as none of this will get touched by the engine in idle state:
		PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);
		
		// Ok, everything sane, fill the buffer:
		buffersize = sizeof(float) * inchannels * insamples;
		if (audiodevices[pahandle].outputbuffer && (audiodevices[pahandle].outputbuffersize != buffersize)) {
			free(audiodevices[pahandle].outputbuffer);
			audiodevices[pahandle].outputbuffer = NULL;
			audiodevices[pahandle].outputbuffersize = 0;
		}
		
		if (audiodevices[pahandle].outputbuffer == NULL) {
			audiodevices[pahandle].outputbuffersize = buffersize;
			audiodevices[pahandle].outputbuffer = (float*) malloc(buffersize);
			if (audiodevices[pahandle].outputbuffer==NULL) PsychErrorExitMsg(PsychError_outofMemory, "Out of system memory when trying to allocate audio buffer.");
		}
		
		// Reset play position:
		audiodevices[pahandle].playposition = 0;
		
		outdata = audiodevices[pahandle].outputbuffer;
		if (indata) {
			// Copy the data, convert it from double to float:
			while(buffersize) {
				*(outdata++) = (float) *(indata++);
				buffersize-=sizeof(float);
			}
		}
		else {
			// Data copy from internal audio buffer (already in float format):
			memcpy(outdata, indatafloat, buffersize);
		}
		
		// Reset write position to end of buffer:
		audiodevices[pahandle].writeposition = inchannels * insamples;
		
		// Elapsed count of played out samples must be zero as engine is stopped and will restart sometime after this call:
		totalplaycount = 0;
		
		// Current playout time undefined when playback is stopped:
		currentTime = PsychGetNanValue();
		
		// Reset playback loop to full buffer:
		audiodevices[pahandle].loopStartFrame = 0;
		audiodevices[pahandle].loopEndFrame = (audiodevices[pahandle].outputbuffersize / sizeof(float) / audiodevices[pahandle].outchannels) - 1;
	}
	else {
		// Streaming refill while playback is running:

		// Engine stopped? [No need to mutex-lock, as engine can't change from state 0 to other state without our intervention]
		if (audiodevices[pahandle].state == 0) PsychErrorExitMsg(PsychError_user, "Audiodevice not in playback mode! Can't do a streaming buffer refill while stopped.");

		// No buffer allocated? [No need to mutex-lock, see above]
		if (audiodevices[pahandle].outputbuffer == NULL) PsychErrorExitMsg(PsychError_user, "No audio buffer allocated! You must call this method once before start of playback to initially allocate a buffer of sufficient size.");

		// Buffer of sufficient size for a streaming refill of this amount?
		buffersize = sizeof(float) * inchannels * insamples;
		if (audiodevices[pahandle].outputbuffersize < buffersize) PsychErrorExitMsg(PsychError_user, "Total capacity of audio buffer is too small for a refill of this size! Allocate an initial buffer of at least the size of the biggest refill.");

		// Need to lock b'cause of 'playposition':
		PsychPALockDeviceMutex(&audiodevices[pahandle]);

		// Check for buffer underrun:
		if (audiodevices[pahandle].writeposition < audiodevices[pahandle].playposition) underrun = 1;

		// Boundary conditions met. Can we refill immediately or do we need to wait for playback
		// position to progress far enough?
		while ((audiodevices[pahandle].state > 0) && ((audiodevices[pahandle].outputbuffersize / sizeof(float)) - (audiodevices[pahandle].writeposition - audiodevices[pahandle].playposition) - inchannels) <= (inchannels * insamples)) {
			// Sleep a bit, drop the lock throughout sleep:
			PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);
			// TODO: We could do better here by predicting how long it will take at least until we're ready to refill,
			// but a perfect solution would require quite a bit of effort... ...Something for a really boring afternoon.
			PsychYieldIntervalSeconds(yieldInterval);
			PsychPALockDeviceMutex(&audiodevices[pahandle]);
		} 

		// Exit with lock held...
		
		// Have we left the while-loop because the engine stopped? In that case we won't
		// be able to ever get the needed headroom and need to error-out:
		if (audiodevices[pahandle].state == 0) {
			// Ohoh...
			PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);			
			PsychErrorExitMsg(PsychError_user, "Audiodevice no longer in playback mode (Auto stopped?!?)! Can't continue a streaming buffer refill while stopped. Check your code!");
		}
		
		// Ok, device locked and enough headroom for batch streaming refill:
		
		// Copy the data, convert it from double to float, take ringbuffer wraparound into account:
		if (indata) {
			while(buffersize > 0) {
				// Fetch next sample and copy it to matrix:
				audiodevices[pahandle].outputbuffer[(audiodevices[pahandle].writeposition % (audiodevices[pahandle].outputbuffersize / sizeof(float)))] = (float) *(indata++);
				
				// Update sample write counter:
				audiodevices[pahandle].writeposition++;
				
				// Decrement copy counter:
				buffersize-=sizeof(float);
			}
		}
		else {
			// Data copy from internal audio buffer (already in float format):
			while(buffersize > 0) {
				// Fetch next sample and copy it to matrix:
				audiodevices[pahandle].outputbuffer[(audiodevices[pahandle].writeposition % (audiodevices[pahandle].outputbuffersize / sizeof(float)))] = *(indatafloat++);
				
				// Update sample write counter:
				audiodevices[pahandle].writeposition++;
				
				// Decrement copy counter:
				buffersize-=sizeof(float);
			}		
		}
		
		// Retrieve total count of played out samples from engine:
		totalplaycount = audiodevices[pahandle].totalplaycount;

		// Retrieve corresponding timestamp of last playout:
		currentTime = audiodevices[pahandle].currentTime;
		
		// Check for buffer underrun:
		if (audiodevices[pahandle].writeposition < audiodevices[pahandle].playposition) underrun = 1;
		
		// Drop lock here, no longer needed:
		PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);			

		if ((underrun > 0) && (verbosity > 1)) printf("PsychPortAudio-WARNING: Underrun of audio playback buffer detected during streaming refill. Some sound data will be skipped!\n");
	}

	// Copy out underrun flag:
	PsychCopyOutDoubleArg(1, FALSE, (double) underrun);
	
	// Copy out number of submitted sample frames, aka the absolute sample frame startindex for next 'Fillbuffer' call.
	PsychCopyOutDoubleArg(2, FALSE, (double) audiodevices[pahandle].writeposition / (double) audiodevices[pahandle].outchannels);
	
	// Compute and return predicted estimated time of arrival for playout of first sample for next 'FillBuffer' call, aka almost of the last sample in this 'FillBuffer' call:
	etaSecs = currentTime + ((((double) audiodevices[pahandle].writeposition - (double) totalplaycount) / (double) audiodevices[pahandle].outchannels) / (double) audiodevices[pahandle].streaminfo->sampleRate);
	PsychCopyOutDoubleArg(3, FALSE, etaSecs);
	
	// Buffer ready.
	return(PsychError_none);
}

/* PsychPortAudio('RefillBuffer') - Refill existing audio outputbuffer of a device with data.
 */
PsychError PSYCHPORTAUDIORefillBuffer(void) 
{
 	static char useString[] = "PsychPortAudio('RefillBuffer', pahandle [, bufferhandle=0], bufferdata [, startIndex=0]);";
	static char synopsisString[] = 
		"Refill part of an audio data playback buffer of a PortAudio audio device. 'pahandle' is the handle of the device "
		"whose buffer is to be filled. 'bufferhandle' is the handle of the buffer: Use a handle of zero for the standard "
		"buffer created and accessed via 'FillBuffer'. 'bufferdata' is a Matlab double matrix with audio data in double "
		"format. Each row of the matrix specifies one sound channel, each column one sample for each channel. Only floating point "
		"values in double precision are supported. Samples need to be in range -1.0 to +1.0, 0.0 for silence. This is "
		"intentionally a very restricted interface. For lowest latency and best timing we want you to provide audio "
		"data exactly at the optimal format and sample rate, so the driver can safe computation time and latency for "
		"expensive sample rate conversion, sample format conversion, and bounds checking/clipping.\n"
		"Instead of a matrix, you can also pass in the bufferhandle of an audio buffer as 'bufferdata'. This buffer "
		"must have been created beforehand via PsychPortAudio('CreateBuffer', ...). Its content must satisfy the "
		"same constraints as in case of passing a Matlab matrix. The content will be copied from the given buffer "
		"to the standard audio buffer, so it is safe to delete that source buffer if you want.\n"		
		"'startIndex' optional: Defines the first sample frame within the buffer where refill should start. "
		"By default, refilling starts at the beginning of the buffer - at sample frame 0. 'startIndex' allows to "
		"start refilling at some offset.\n"
		"Please note that 'RefillBuffer' can't resize an existing buffer - you can't fill in more data than the "
		"current buffer capacity permits. If you want to add more sound, you'll need to use 'FillBuffer' or "
		"create a new buffer of proper capacity.\n"
		"'RefillBuffer' can be used any time on a buffer, even if the buffer is currently playing, allowing for "
		"on-the-fly replacement of content. However be careful to avoid the currently played section, or you'll "
		"hear audio artifacts. For streaming out audio content in a glitch-free way, you may want to use the "
		"'streamingrefill' option of the 'FillBuffer' subfunction instead.\n";

	static char seeAlsoString[] = "Open FillBuffer GetStatus ";	 
  	
	PsychPABuffer* buffer;
	PsychPABuffer* inbuffer;
	
	int inchannels, insamples, p, buffersize, outbuffersize;
	double*	indata = NULL;
	int inbufferhandle = 0;
	float*  indatafloat = NULL;
	float*  outdata = NULL;
	int pahandle   = -1;
	int bufferhandle = 0;
	int startIndex = 0;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(4));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(3)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(0));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");
	if ((audiodevices[pahandle].opmode & kPortAudioPlayBack) == 0) PsychErrorExitMsg(PsychError_user, "Audio device has not been opened for audio playback, so this call doesn't make sense.");

	PsychCopyInIntegerArg(2, kPsychArgOptional, &bufferhandle);
	// Check for valid bufferhandle:
	if (bufferhandle < 0) PsychErrorExitMsg(PsychError_user, "Invalid audio 'bufferhandle' provided.");

	// If it is a non-zero handle, try to dereference from dynamic buffer:
	if (bufferhandle > 0) {
		// Deref bufferHandle: Issue error if no buffer with such a handle exists:
		buffer = PsychPAGetAudioBuffer(bufferhandle);
		
		// Validate matching output channel count:
		if (buffer->outchannels != audiodevices[pahandle].outchannels) {
			printf("PsychPortAudio-ERROR: Audio channel count %i of audiobuffer with handle %i doesn't match channel count %i of audio device!\n", buffer->outchannels, bufferhandle, audiodevices[pahandle].outchannels);
			PsychErrorExitMsg(PsychError_user, "Target audio buffer 'bufferHandle' has an audio channel count that doesn't match channels of audio device!");
		}
	}

	// Bufferhandle instead of input data matrix provided?
	if (PsychCopyInIntegerArg(3, kPsychArgAnything, &inbufferhandle) && (inbufferhandle > 0)) {
		// Seems so. Double check:
		inbuffer = PsychPAGetAudioBuffer(inbufferhandle);
		
		// Assign properties:
		inchannels = inbuffer->outchannels;
		insamples = inbuffer->outputbuffersize / sizeof(float) / inchannels;
		p = 1;
		indatafloat = inbuffer->outputbuffer;
	}
	else {
		// Regular double matrix with sound data from runtime:
		PsychAllocInDoubleMatArg(3, kPsychArgRequired, &inchannels, &insamples, &p, &indata);
	}
	
	if (inchannels != audiodevices[pahandle].outchannels) {
		printf("PTB-ERROR: Audio device %i has %i output channels, but provided matrix has non-matching number of %i rows.\n", pahandle, audiodevices[pahandle].outchannels, inchannels);
		PsychErrorExitMsg(PsychError_user, "Number of rows of audio data matrix doesn't match number of output channels of selected audio device.\n");
	}
	
	if (insamples < 1) PsychErrorExitMsg(PsychError_user, "You must provide at least 1 sample for refill of your audio buffer!");
	if (p!=1) PsychErrorExitMsg(PsychError_user, "Audio data matrix must be a 2D matrix, but this one is not a 2D matrix!");
	
	// Get optional startIndex:
	PsychCopyInIntegerArg(4, kPsychArgOptional, &startIndex);
	if (startIndex < 0) PsychErrorExitMsg(PsychError_user, "Invalid 'startIndex' provided. Must be greater or equal to zero.");

	// Assign bufferpointer based on bufferhandle:
	if (bufferhandle > 0) {
		// Generic buffer:
		outdata = buffer->outputbuffer;
		outbuffersize = buffer->outputbuffersize;
	}
	else {
		// Standard playout buffer:
		outdata = audiodevices[pahandle].outputbuffer;
		outbuffersize = audiodevices[pahandle].outputbuffersize;
	}

	// Buffer exists?
	if (outdata == NULL) PsychErrorExitMsg(PsychError_user, "No such buffer with given 'bufferhandle', or buffer not yet created!");
	
	// Compute required buffersize for copying all data from given startIndex:
	buffersize = sizeof(float) * inchannels * (insamples + startIndex);
	
	// Buffer of sufficient size?
	if (buffersize > outbuffersize) {
		// Nope, too small: Adapt 'buffersize' to allowable maximum amount:
		if (verbosity > 1) printf("PsychPortAudio: WARNING: In 'RefillBuffer' for bufferhandle %i at startindex %i: Insufficient\nbuffersize %i for %i new audioframes starting at given startindex.\nWill truncate to maximum possible.\n", bufferhandle, startIndex, outbuffersize / (sizeof(float) * inchannels), insamples);
		buffersize = outbuffersize;
		buffersize -= sizeof(float) * inchannels * startIndex;
	}
	else {
		// Big enough:
		buffersize = sizeof(float) * inchannels * insamples;
	}
	
	// Map startIndex to offset in buffer:
	outdata += inchannels * startIndex;
	
	// Ok, everything sane, fill the buffer: 'buffersize' iterations into 'outdata':
	//fprintf(stderr, "buffersize = %i\n", buffersize);

	if (indata) {
		// Copy the data, convert it from double to float:
		while(buffersize > 0) {
			*(outdata++) = (float) *(indata++);
			buffersize-=sizeof(float);
		}
	}
	else {
		// Data copy from internal audio buffer (already in float format):
		memcpy(outdata, indatafloat, buffersize);
	}

	// Done.
	return(PsychError_none);
}

/* PsychPortAudio('DeleteBuffer') - Delete a dynamic audio outputbuffer.
 */
PsychError PSYCHPORTAUDIODeleteBuffer(void) 
{
 	static char useString[] = "result = PsychPortAudio('DeleteBuffer'[, bufferhandle] [, waitmode]);";
	static char synopsisString[] = 
		"Delete an existing dynamic audio data playback buffer.\n"
		"'bufferhandle' is the handle for the buffer to delete. If it is omitted, all "
		"buffers will be deleted. 'waitmode' defines what happens if a buffer shall be "
		"deleted that is currently in use, i.e., part of the audio playback schedule "
		"of an active audio device. The default of zero will simply return without deleting "
		"the buffer. A setting of 1 will wait until the buffer can be safely deleted.\n";

	static char seeAlsoString[] = "Open FillBuffer GetStatus ";	 
  	
	int bufferhandle = 0;
	int waitmode = 0;
	int rc = 0;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(2));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	// Get optional waitmode with default zero:
	PsychCopyInIntegerArg(2, FALSE, &waitmode);

	if (PsychCopyInIntegerArg(1, FALSE, &bufferhandle)) {
		// Specific bufferhandle provided for deletion:
		
		// Check if handle valid and error out if so:
		(void) PsychPAGetAudioBuffer(bufferhandle);
		
		// Valid: Try to delete buffer:
		rc = PsychPADeleteAudioBuffer(bufferhandle, waitmode);
	}
	else {
		// No specific handle: Try to delete all buffers:
		if (PsychPAUpdateBufferReferences()) {
			// At least one buffer locked. What do do?
			if (waitmode == 0) {
				// Just fail -> No op.
				rc = 0;
			}
			else {
				// Retry until it works:
				while (PsychPAUpdateBufferReferences()) PsychYieldIntervalSeconds(yieldInterval);
				rc = 1;
			}
		}
		else {
			rc = 1;
		}
		
		// Really delete all buffers if rc == 1:
		if (rc == 1) PsychPADeleteAllAudioBuffers();
	}
	
	// Return status:
	PsychCopyOutDoubleArg(1, FALSE, (double) rc);

	// Done.
	return(PsychError_none);
}

/* PsychPortAudio('CreateBuffer') - Create and fill dynamic audio outputbuffer of a device with data.
 */
PsychError PSYCHPORTAUDIOCreateBuffer(void) 
{
 	static char useString[] = "bufferhandle = PsychPortAudio('CreateBuffer' [, pahandle], bufferdata);";
	static char synopsisString[] = 
		"Create a new dynamic audio data playback buffer for a PortAudio audio device and fill it with initial data.\n"
		"Return a 'bufferhandle' to the new buffer. 'pahandle' is the optional handle of the device "
		"whose buffer is to be filled. 'bufferdata' is a Matlab double matrix with audio data in double "
		"format. Each row of the matrix specifies one sound channel, each column one sample for each channel. Only floating point "
		"values in double precision are supported. Samples need to be in range -1.0 to +1.0, 0.0 for silence. This is "
		"intentionally a very restricted interface. For lowest latency and best timing we want you to provide audio "
		"data exactly at the optimal format and sample rate, so the driver can safe computation time and latency for "
		"expensive sample rate conversion, sample format conversion, and bounds checking/clipping.\n\n"
		"You can refill the buffer anytime via the PsychPortAudio('RefillBuffer') call.\n"
		"You can delete the buffer via the PsychPortAudio('DeleteBuffer') call, once it is not used anymore. \n"
		"You can attach the buffer to an audio playback schedule for actual audio playback via the "
		"PsychPortAudio('AddToSchedule') call.\n"
		"The same buffer can be attached to and used by multiple audio devices simultaneously, or multiple "
		"times within one or more playback schedules. ";

	static char seeAlsoString[] = "Open FillBuffer GetStatus ";	 
  	
	PsychPABuffer* buffer;
	int inchannels, insamples, p, buffersize, outbuffersize;
	double*	indata = NULL;
	float*  outdata = NULL;
	int pahandle   = -1;
	int bufferhandle = 0;
	int startIndex = 0;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(2));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	// Get data matrix with initial buffer content:
	PsychAllocInDoubleMatArg(2, kPsychArgRequired, &inchannels, &insamples, &p, &indata);

	// If the optional pahandle is provided...
	if (PsychCopyInIntegerArg(1, kPsychArgOptional, &pahandle)) {
		// ...then we use it to validate the configuration of the datamatrix for the new
		// buffer against the requirements of that audiodevice pahandle:
		if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");
		if ((audiodevices[pahandle].opmode & kPortAudioPlayBack) == 0) PsychErrorExitMsg(PsychError_user, "Audio device has not been opened for audio playback, so this call doesn't make sense.");
		
		if (inchannels != audiodevices[pahandle].outchannels) {
			printf("PTB-ERROR: Audio device %i has %i output channels, but provided matrix has non-matching number of %i rows.\n", pahandle, audiodevices[pahandle].outchannels, inchannels);
			PsychErrorExitMsg(PsychError_user, "Number of rows of audio data matrix doesn't match number of output channels of selected audio device.\n");
		}
	}

	if (inchannels < 1) PsychErrorExitMsg(PsychError_user, "You must provide at least 1 row for creation of at least one audio channel in your audio buffer!");
	if (insamples < 1) PsychErrorExitMsg(PsychError_user, "You must provide at least 1 sample for creation of your audio buffer!");
	if (p!=1) PsychErrorExitMsg(PsychError_user, "Audio data matrix must be a 2D matrix, but this one is not a 2D matrix!");

	// Create buffer and assign bufferhandle:
	bufferhandle = PsychPACreateAudioBuffer(inchannels, insamples);
	
	// Deref bufferHandle:
	buffer = PsychPAGetAudioBuffer(bufferhandle);
	outdata = buffer->outputbuffer;
	outbuffersize = buffer->outputbuffersize;
	buffersize = sizeof(float) * inchannels * insamples;
		
	// Copy the data, convert it from double to float:
	while(buffersize > 0) {
		*(outdata++) = (float) *(indata++);
		buffersize-=sizeof(float);
	}
	
	// Return bufferhandle:
	PsychCopyOutDoubleArg(1, FALSE, (double) bufferhandle);

	// Done.
	return(PsychError_none);
}

/* PsychPortAudio('GetAudioData') - Retrieve captured audio data.
 */
PsychError PSYCHPORTAUDIOGetAudioData(void) 
{
 	static char useString[] = "[audiodata absrecposition overflow cstarttime] = PsychPortAudio('GetAudioData', pahandle [, amountToAllocateSecs][, minimumAmountToReturnSecs][, maximumAmountToReturnSecs]);";
	static char synopsisString[] = 
		"Retrieve captured audio data from a audio device. 'pahandle' is the handle of the device "
		"whose data is to be retrieved. 'audiodata' is a Matlab double matrix with audio data in double format. Each "
		"row of the matrix returns one sound channel, each column one sample for each channel. Only floating point "
		"values in double precision are returned. Samples will be in range -1.0 to +1.0, 0.0 for silence. This is "
		"intentionally a very restricted interface. For lowest latency and best timing we want you to accept audio "
		"data exactly at the optimal format and sample rate, so the driver can safe computation time and latency for "
		"expensive sample rate conversion, sample format conversion, and bounds checking/clipping.\n"
		"You must call this function once before start of capture operations to allocate an internal buffer "
		"that stores captured audio data inbetween your periodic calls. Provide 'amountToAllocateSecs' as "
		"requested buffersize in seconds. After start of capture you must call this function periodically "
		"at least every 'amountToAllocateSecs' seconds to drain the internal buffer into your Matlab/Octave "
		"matrix 'audiodata'. If you fail to call the function frequently enough, sound data will get lost!\n"
		"'minimumAmountToReturnSecs' optional minimum amount of recorded data to return at each call. The "
		"driver will only return control to your script when it was able to collect at least that amount "
		"of seconds of sound data - or if the capture engine was stopped. If you don't set this parameter, "
		"the driver will return immediately, giving you whatever amount of sound data was available - including "
		"an empty matrix if nothing was available.\n"
		"'maximumAmountToReturnSecs' allows you to optionally restrict the amount of returned sound data to "
		"a specific duration in seconds. By default, you'll get whatever is available.\n"
		"If you provide both, 'minimumAmountToReturnSecs' and 'maximumAmountToReturnSecs' and set them to equal "
		"values (but significantly lower than the 'amountToAllocateSecs' buffersize!!) then you'll always "
		"get an 'audiodata' matrix back that is of a fixed size. This may be convenient for postprocessing "
		"in Matlab. It may also reduce or avoid Matlab memory fragmentation...\n\n"
		"\nOptional return arguments other than 'audiodata':\n\n"
		"'absrecposition' is the absolute position (in samples) of the first column in the returned data matrix, "
		"assuming that sample zero was the very first recorded sample in this session. The count is reset each time "
		"you start a new capture session via call to PsychPortAudio('Start').\n"
		"Each call to this function will return a new chunk of recorded sound data. The 'absrecposition' provides "
		"you with absolute matrix column indices to stitch together the results of all calls into one seamless "
		"recording if you want. 'overflow' if this flag is zero then everything went fine. If it is one then you "
		"didn't manage to call this function frequent enough, the capacity of the internal recording buffer was "
		"exceeded and therefore you lost captured sound data, i.e., there is a gap in your recording. When "
		"initially allocating the internal buffer, make sure to allocate it big enough so it is able to easily "
		"store all recorded data inbetween your calls to 'GetAudioData'. Example: You expect to call this routine "
		"once every second in your trial loop, then allocate a sound buffer of at least 2 seconds for some security "
		"headroom. If you know that the recording time of each recording has an upper bound then you can allocate "
		"an internal buffer of sufficient size and fetch the buffer all at once at the end of a recording.\n"
		"'ctsstarttime' this is an estimate of the system time (in seconds) when the very first sample of this "
		"recording was captured by the sound input of your hardware. This is only a rough estimate, not to be "
		"trusted down to the millisecond level, at least not without former careful calibration of your setup!\n";

	static char seeAlsoString[] = "Open GetDeviceSettings ";	 
  	
	int inchannels, insamples, p, buffersize, maxSamples;
	double*	indata = NULL;
	float*  outdata = NULL;
	int pahandle   = -1;
	double allocsize;
	double minSecs, maxSecs, minSamples;
	int overrun = 0;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(4));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(4));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");
	if ((audiodevices[pahandle].opmode & kPortAudioCapture) == 0) PsychErrorExitMsg(PsychError_user, "Audio device has not been opened for audio capture, so this call doesn't make sense.");

	buffersize = audiodevices[pahandle].inputbuffersize;
	
	// Copy in optional amount of buffer memory to allocate for internal recording ringbuffer:
	allocsize = 0;
	PsychCopyInDoubleArg(2, kPsychArgOptional, &allocsize);

	// Internal capture ringbuffer already allocated?
	if (buffersize == 0) {
		// Nope. This call needs to allocate it. We know the engine is idle in this case, because it
		// can't be started or brought out of idle if buffersize == 0 due to checks in 'Start' and 'RescheduleStart'.
		if (allocsize <= 0) PsychErrorExitMsg(PsychError_user, "You must first call this function with a positive 'amountToAllocateSecs' argument to allocate internal bufferspace first!");
		
		// Ready to allocate ringbuffer memory outside this if-clause...
	}
	else {
		// Buffer already allocated. Need to realloc?
		if (allocsize > 0) {
			// Calling script wants to reallocate the buffer. Check if this is possible here:
			
			// Test 1: Engine running? [No need to mutex lock: If running, we fail safely. If state = 0 it can't transition to > 0 behind our back]
			if (audiodevices[pahandle].state > 0) PsychErrorExitMsg(PsychError_user, "Tried to resize internal buffer while recording engine is running! You must stop recording before resizing the buffer!");

			// Test 2: Pending samples to read from current ringbuffer? Engine is idle, so we can safely access device data lock-free...
			if (audiodevices[pahandle].readposition < audiodevices[pahandle].recposition) PsychErrorExitMsg(PsychError_user, "Tried to resize internal buffer without emptying it beforehand. You must drain the buffer before resizing it!");

			// Ok, reallocation allowed, as engine is idle. Delete old buffer:
			audiodevices[pahandle].inputbuffersize = 0;
			free(audiodevices[pahandle].inputbuffer);
			audiodevices[pahandle].inputbuffer = NULL;
			
			// At this point we are ready to re-allocate ringbuffer outside this if-clause...
		}
	}
	
	// Still (re-)allocation wanted?
	if (allocsize > 0) {
		// We know the engine is idle if we reach this point, so no need to acquire locks or check state...
		
		// Calculate needed buffersize in samples: Convert allocsize in seconds to size in bytes:
		audiodevices[pahandle].inputbuffersize = sizeof(float) * ((int) (allocsize * audiodevices[pahandle].streaminfo->sampleRate)) * audiodevices[pahandle].inchannels;
		audiodevices[pahandle].inputbuffer = (float*) calloc(1, audiodevices[pahandle].inputbuffersize);
		if (audiodevices[pahandle].inputbuffer == NULL) PsychErrorExitMsg(PsychError_outofMemory, "Free system memory exhausted when trying to allocate audio recording buffer!");

		// This was an (re-)allocation call, so no data is pending in the buffer.
		// Therefore we don't return any data, just reset the counters:
		audiodevices[pahandle].recposition = 0;
		audiodevices[pahandle].readposition = 0;
		return(PsychError_none);
	}
	
	// This is not an allocation call, but a real data fetch call:

	// Get optional "minimum amount to return" argument:
	// We default to "whatever we can get" ie. zero seconds.
	minSecs = 0;
	PsychCopyInDoubleArg(3, kPsychArgOptional, &minSecs);

	// Get optional "maximum amount to return" argument:
	// We default to "whatever we can get" ie. infinite seconds.
	maxSecs = 0;
	PsychCopyInDoubleArg(4, kPsychArgOptional, &maxSecs);

	// The engine is potentially running, so we need to mutex-lock our accesses...
	PsychPALockDeviceMutex(&audiodevices[pahandle]);

	// How much samples are available in ringbuffer to fetch?
	insamples = audiodevices[pahandle].recposition - audiodevices[pahandle].readposition;
	
	// Convert amount of available data into seconds and check if our minimum
	// requirements are fulfilled:
	if (minSecs > 0) {
		// Convert seconds to samples:
		minSamples = minSecs * ((double) audiodevices[pahandle].streaminfo->sampleRate) * ((double) audiodevices[pahandle].inchannels) + ((double) audiodevices[pahandle].inchannels);

		// Bigger than buffersize? That would be a no no...
		if ((minSamples * sizeof(float)) > audiodevices[pahandle].inputbuffersize) {
			PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);
			PsychErrorExitMsg(PsychError_user, "Invalid 'minimumAmountToReturnSecs' parameter: The requested minimum is bigger than the whole capture buffer size!'");			
		}
		
		// Loop until either request is fullfillable or the device gets stopped - in which
		// case we'll never be able to fullfill the request...
		while (((double) insamples < minSamples) && (audiodevices[pahandle].state > 0)) {
			// Compute amount of time to elapse before request could be fullfilled:
			minSecs = (minSamples - (double) insamples) / ((double) audiodevices[pahandle].inchannels) / ((double) audiodevices[pahandle].streaminfo->sampleRate);
			// Ok, required data will be available earliest in 'minSecs' seconds. Sleep until then with lock dropped:
			PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);
			PsychWaitIntervalSeconds(minSecs);
			PsychPALockDeviceMutex(&audiodevices[pahandle]);
			
			// We've slept at least the estimated amount of required time. Recalculate amount
			// of available sound data and check again...
			insamples = audiodevices[pahandle].recposition - audiodevices[pahandle].readposition;
		}
	}
	
	// Lock held here...
	
	// Never ever fetch the samples for the last sampleframe. We do not want to fetch
	// a possibly not yet updated or incomplete sample frame. Leave this to next call
	// of this function. Well, unless state is zero == engine stopped. In that case we
	// know that the playhead won't move anymore and we can safely fetch all remaining
	// data.
	if (audiodevices[pahandle].state > 0) {
		insamples = insamples - (insamples % audiodevices[pahandle].inchannels);
		insamples-= audiodevices[pahandle].inchannels;
	}

	// Can unlock here: The remainder of the routine doesn't touch any critical device variables anymore,
	// only variables that aren't modified by the engine, or not used/touched by engine.
	// Well, theoretically the engine could overwrite the portion of the buffer we're going to
	// read out if we stall massively and the capturebuffer is way too "undersized", but in that
	// case the user code is screwed anyway and it (or the system) needs to be fixed...
	PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);
	
	insamples = (insamples < 0) ? 0 : insamples;
	buffersize = insamples * sizeof(float);

	// Buffer "overflow" detected?
	if (buffersize > audiodevices[pahandle].inputbuffersize) {
		// Ok, the buffer did overrun and captured data was lost. Limit returned data
		// to buffersize and set the overrun flag, optionally output a warning:
		buffersize = audiodevices[pahandle].inputbuffersize;
		insamples = buffersize / sizeof(float);
		
		// Set overrun flag:
		overrun = 1;
		
		if (verbosity > 1) printf("PsychPortAudio-WARNING: Overflow of audio capture buffer detected. Some sound data will be lost!\n");
	}
	
	// Limitation of returned amount of data wanted?
	if (maxSecs > 0) {
		// Yes. Convert maximum amount in seconds to maximum amount in samples:
		maxSamples = (int) (ceil(maxSecs * ((double) audiodevices[pahandle].streaminfo->sampleRate)) * ((double) audiodevices[pahandle].inchannels));
		// Clamp insamples to that value, if neccessary:
		if (insamples > maxSamples) {
			insamples = maxSamples;
			buffersize = insamples * sizeof(float);
		}
	}
	
	// Allocate output double matrix with matching number of channels and samples:
	PsychAllocOutDoubleMatArg(1, FALSE, audiodevices[pahandle].inchannels, insamples / audiodevices[pahandle].inchannels, 1, &indata);

	// Copy out absolute sample read position of first sample in buffer:
	PsychCopyOutDoubleArg(2, FALSE, (double) (audiodevices[pahandle].readposition / audiodevices[pahandle].inchannels));

	// Copy the data, convert it from float to double: Take ringbuffer wraparound into account:
	while(buffersize > 0) {
		// Fetch next sample and copy it to matrix:
		*(indata++) = (double) audiodevices[pahandle].inputbuffer[(audiodevices[pahandle].readposition % (audiodevices[pahandle].inputbuffersize / sizeof(float)))];

		// Update sample read counter:
		audiodevices[pahandle].readposition++;
		
		// Decrement copy counter:
		buffersize-=sizeof(float);
	}

	// Copy out overrun flag:
	PsychCopyOutDoubleArg(3, FALSE, (double) overrun);

	// Return capture timestamp in system time of first captured sample in this session. This is a bit problematic:
	// In full-duplex mode, at least OS/X doesn't return separate timestamps, so we'll provide the playback onset time
	// instead - the best we can do. In pure capture mode we get a capture timestamp...
	PsychCopyOutDoubleArg(4, FALSE, (audiodevices[pahandle].captureStartTime > 0) ? audiodevices[pahandle].captureStartTime : audiodevices[pahandle].startTime);
	
	// Buffer ready.
	return(PsychError_none);
}

/* PsychPortAudio('RescheduleStart') - Set new start time for an already running audio device via PortAudio.
 */
PsychError PSYCHPORTAUDIORescheduleStart(void) 
{
 	static char useString[] = "startTime = PsychPortAudio('RescheduleStart', pahandle, when [, waitForStart=0] [, repetitions] [, stopTime]);";
	static char synopsisString[] = 
		"Modify requested start time 'when' of an already started PortAudio audio device.\n"
		"After you've started an audio device via the 'Start' subfunction, but *before* the "
		"device has really started playback (because the 'when' time provided to the 'Start' "
		"method is still far in the future), you can use this function to reschedule the start for "
		"a different 'when' time - including a value of zero for immediate start.\n"
		"\n"
		"The 'pahandle' is the handle of the device to start. Starting a "
		"device means: Start playback of output devices, start recording on capture device, do both on "
		"full duplex devices. 'waitForStart' if set to 1 will wait until device has really started, default "
		"is to continue immediately, ie. only schedule start of device. 'when' Requested time, when device "
		"should start. Defaults to zero, i.e. start immediately. If set to a non-zero system time, PTB will "
		"do its best to start the device at the requested time, but the accuracy of start depends on the "
		"operating system, audio hardware and system load. If 'waitForStart' is set to non-zero value, ie "
		"if PTB should wait for sound onset, then the optional return argument 'startTime' will contain an "
		"estimate of when the first audio sample hit the speakers, i.e., the real start time.\n"
		"Please note that the 'when' value always refers to playback, so it defines the starttime of "
		"playback. The start time of capture is related to the start time of playback in duplex mode, "
		"but it isn't the same. In pure capture mode (without playback), 'when' will be ignored and "
		"capture always starts immediately. See the help for subfunction 'GetStatus' for more info on "
		"the meaning of the different timestamps.\n"
		"The 'repetitions' parameter will change the number of playback repetitions if provided. The "
		"value for 'repetitions' from the PsychPortAudio('Start') function will be used if the parameter "
		"is omitted. See explanation in the 'Start' function for its meaning.\n"
		"'stopTime' is an optional override for the 'stopTime' parameter from the 'Start' function, see "
		"explanations there.\n";

	static char seeAlsoString[] = "Open";	 
  	
	PaError err;
	int pahandle= -1;
	int waitForStart = 0;
	double when = 0.0;
	double repetitions = -1;
	double stopTime = -1;
	 
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(5));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(2)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");
	if ((audiodevices[pahandle].opmode & kPortAudioMonitoring) == 0) {
		// Not in monitoring mode: We must have in/outbuffers allocated:
		if ((audiodevices[pahandle].opmode & kPortAudioPlayBack) && (audiodevices[pahandle].outputbuffer == NULL) && (audiodevices[pahandle].schedule == NULL)) PsychErrorExitMsg(PsychError_user, "Sound outputbuffer doesn't contain any sound to play?!?");
		if ((audiodevices[pahandle].opmode & kPortAudioCapture) && (audiodevices[pahandle].inputbuffer == NULL)) PsychErrorExitMsg(PsychError_user, "Sound inputbuffer not prepared/allocated for capture?!?");
	}

	// Get new required 'when' start time:
	PsychCopyInDoubleArg(2, kPsychArgRequired, &when);
	if (when < 0) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'when'. Valid values are zero or greater.");

	PsychCopyInIntegerArg(3, kPsychArgOptional, &waitForStart);
	if (waitForStart < 0) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'waitForStart'. Valid values are zero or greater.");

	// Get new optional 'repetitions' count:
	if (PsychCopyInDoubleArg(4, kPsychArgOptional, &repetitions)) {
		// Argument provided: Range-Check and assign it:
		if (repetitions < 0) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'repetitions'. Valid values are zero or greater.");
	}
	else {
		repetitions = -1;
	}

	// Get new optional 'stopTime':
	if (PsychCopyInDoubleArg(5, kPsychArgOptional, &stopTime)) {
		// Argument provided: Range-Check and assign it:
		if (stopTime <= when && (stopTime < DBL_MAX)) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'stopTime'. Valid values are greater than 'when' starttime.");
	}
	else {
		stopTime = -1;
	}

	// Audio engine running? That is the minimum requirement for this function to work:
	if (!Pa_IsStreamActive(audiodevices[pahandle].stream)) PsychErrorExitMsg(PsychError_user, "Audio device not started. You need to call the 'Start' function first!");

	// Lock the device:
	PsychPALockDeviceMutex(&audiodevices[pahandle]);
	
	// Whatever the current scheduled starttime is, override it to be infinity:
	audiodevices[pahandle].reqStartTime = DBL_MAX;
	
	// Reset any pending requests:
	audiodevices[pahandle].reqstate = 255;

	// Engine is running. Is it in a state ready for rescheduling a start?
	
	// In runMode zero it must be in hot-standby as immediately after a 'Start' in order to be reschedulable:
	if ((audiodevices[pahandle].runMode == 0) && (audiodevices[pahandle].state != 1)) {
		PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);
		PsychErrorExitMsg(PsychError_user, "Audio device not started and waiting. You need to call the 'Start' function first with an infinite 'when' time or a 'when' time in the far future!");
	}
	
	// In runMode 1 the device itself is always running and has to be in a logically stopped/idle (=0) state or hotstandby for rescheduling.
	if ((audiodevices[pahandle].runMode == 1) && (audiodevices[pahandle].state > 1)) {
		PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);
		PsychErrorExitMsg(PsychError_user, "Audio device not idle. Make sure it is idle first, e.g., by proper use of the 'Stop' function or by checking its 'Active' state via the 'GetStatus' function!");
	}

	// Audio engine is in a proper state for rescheduling now: 

	// New repetitions provided?
	if (repetitions >=0) {
		// Set number of requested repetitions: 0 means loop forever, default is 1 time.
		audiodevices[pahandle].repeatCount = (repetitions == 0) ? -1 : repetitions;
	}

	// New stopTime provided?
	if (stopTime >= 0) audiodevices[pahandle].reqStopTime = stopTime;

	// Reset statistics:
	audiodevices[pahandle].xruns = 0;	
	audiodevices[pahandle].noTime = 0;
	audiodevices[pahandle].captureStartTime = 0;
	audiodevices[pahandle].startTime = 0.0;
	audiodevices[pahandle].estStopTime = 0;
	audiodevices[pahandle].currentTime = 0;		
	audiodevices[pahandle].schedule_pos = 0;
	
	// Reset recorded samples counter:
	audiodevices[pahandle].recposition = 0;

	// Reset read samples counter: This will discard possibly not yet fetched data.
	audiodevices[pahandle].readposition = 0;

	// Reset play position:
	audiodevices[pahandle].playposition = 0;
	
	// Reset total count of played out samples:
	audiodevices[pahandle].totalplaycount = 0;
	
	// Setup new rescheduled target start time:
	audiodevices[pahandle].reqStartTime = when;

	if (audiodevices[pahandle].runMode == 1) {
		// Set the state to hot-standby to actually make this scheduling request active:
		audiodevices[pahandle].state = 1;
	}

	if (waitForStart>0) {
		// Wait for real start of device: We enter the first while() loop iteration with
		// the device lock still held from above, so the while() loop will iterate at
		// least once...
		while (audiodevices[pahandle].state == 1 && Pa_IsStreamActive(audiodevices[pahandle].stream)) {
			// Wait for a state-change before reevaluating the .state:
			PsychPAWaitForChange(&audiodevices[pahandle]);
		}
		
		// Device has started (potentially even already finished for very short sounds!)
		// In any case we have a valid .startTime to wait for in the device struct,
		// which won't change by itself anymore, so it is safe to access unlocked.
		// Unlock device:
		PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);

		// Ok, relevant audio buffer with real sound onset submitted to engine.
		// We now have an estimate of real sound onset in startTime, wait until
		// then:
		PsychWaitUntilSeconds(audiodevices[pahandle].startTime);
		
		// Engine should run now. Return real onset time:
		PsychCopyOutDoubleArg(1, kPsychArgOptional, audiodevices[pahandle].startTime);
	}
	else {
		// Unlock device: This will trigger actual start at next paCallback() invocation by engine:
		PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);

		// Return empty zero timestamp to signal that this info is not available:
		PsychCopyOutDoubleArg(1, kPsychArgOptional, 0.0);
	}
	
	return(PsychError_none);
}


/* PsychPortAudio('Start') - Start an audio device via PortAudio.
 */
PsychError PSYCHPORTAUDIOStartAudioDevice(void) 
{
 	static char useString[] = "startTime = PsychPortAudio('Start', pahandle [, repetitions=1] [, when=0] [, waitForStart=0] [, stopTime=inf]);";
	static char synopsisString[] = 
		"Start a PortAudio audio device. The 'pahandle' is the handle of the device to start. Starting a "
		"device means: Start playback of output devices, start recording on capture device, do both on "
		"full duplex devices. 'waitForStart' if set to 1 will wait until device has really started, default "
		"is to continue immediately, ie. only schedule start of device. 'when' Requested time, when device "
		"should start. Defaults to zero, i.e. start immediately. If set to a non-zero system time, PTB will "
		"do its best to start the device at the requested time, but the accuracy of start depends on the "
		"operating system, audio hardware and system load. If 'waitForStart' is set to non-zero value, ie "
		"if PTB should wait for sound onset, then the optional return argument 'startTime' will contain an "
		"estimate of when the first audio sample hit the speakers, i.e., the real start time.\n"
		"Please note that the 'when' value always refers to playback, so it defines the starttime of "
		"playback. The start time of capture is related to the start time of playback in duplex mode, "
		"but it isn't the same. In pure capture mode (without playback), 'when' will be ignored and "
		"capture always starts immediately. See the help for subfunction 'GetStatus' for more info on "
		"the meaning of the different timestamps.\n"
		"The 'repetitions' parameter defines how often the playback of the sound data should be repeated. "
		"A setting of zero will cause infinite repetitions, ie., until manually stopped via the 'Stop' "
		"subfunction. A positive setting will cause the provided number of repetitions to happen. The "
		"default setting is 1, ie., play exactly once, then stop. Fractional values are allowed, e.g, "
		"1.5 for one and a half repetition.\n"
		"The optional parameter 'stopTime' allows to set a specific system time when sound playback "
		"should stop by itself at latest, regardless if the requested number of 'repetitions' has "
		"completed. PTB will do its best to stop sound at exactly that time, see comments about the "
		"'when' parameter - The same mechanism is used, with the same restrictions.\n";
		
	static char seeAlsoString[] = "Open";	 
  	
	PaError err;
	int pahandle= -1;
	int waitForStart = 0;
	double repetitions = 1;
	double when = 0.0;
	double stopTime = DBL_MAX;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(5));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");

	PsychCopyInDoubleArg(2, kPsychArgOptional, &repetitions);
	if (repetitions < 0) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'repetitions'. Valid values are zero or greater.");

	PsychCopyInDoubleArg(3, kPsychArgOptional, &when);
	if (when < 0) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'when'. Valid values are zero or greater.");

	PsychCopyInIntegerArg(4, kPsychArgOptional, &waitForStart);
	if (waitForStart < 0) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'waitForStart'. Valid values are zero or greater.");

	PsychCopyInDoubleArg(5, kPsychArgOptional, &stopTime);
	if (stopTime <= when && (stopTime < DBL_MAX)) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'stopTime'. Valid values are greater than 'when' starttime.");

	if ((audiodevices[pahandle].opmode & kPortAudioMonitoring) == 0) {
		// Not in monitoring mode: We must have in/outbuffers allocated:
		if ((audiodevices[pahandle].opmode & kPortAudioPlayBack) && (audiodevices[pahandle].outputbuffer == NULL) && (audiodevices[pahandle].schedule == NULL)) PsychErrorExitMsg(PsychError_user, "Sound outputbuffer doesn't contain any sound to play?!?");
		if ((audiodevices[pahandle].opmode & kPortAudioCapture) && (audiodevices[pahandle].inputbuffer == NULL)) PsychErrorExitMsg(PsychError_user, "Sound inputbuffer not prepared/allocated for capture?!?");
	}

	// Make sure current state is zero, aka fully stopped and engine is really stopped: Output a warning if this looks like an
	// unintended "too early" restart: [No need to mutex-lock here, as iff these .state setting is not met,
	// then we are good and they can't change by themselves behind our back -- paCallback() can't change .state to > 0]
	if ((audiodevices[pahandle].state > 0) && Pa_IsStreamActive(audiodevices[pahandle].stream)) {
		if (verbosity > 1) {
			printf("PsychPortAudio-WARNING: 'Start' method on audiodevice %i called, although playback on device not yet completely stopped.\nWill forcefully restart with possible audible artifacts or timing glitches.\nCheck your playback timing or use the 'Stop' function properly!\n", pahandle);
		}
	}

	// Safeguard: If the stream is not stopped in runMode 0, do it now:
	if (!Pa_IsStreamStopped(audiodevices[pahandle].stream)) {
		if (audiodevices[pahandle].runMode == 0) Pa_StopStream(audiodevices[pahandle].stream);
	}
	
	// Mutex-lock here: Needed if engine already/still running in runMode1, doesn't hurt if engine is stopped
	PsychPALockDeviceMutex(&audiodevices[pahandle]);

	// Reset statistics values:
	audiodevices[pahandle].batchsize = 0;	
	audiodevices[pahandle].xruns = 0;	
	audiodevices[pahandle].paCalls = 0;
	audiodevices[pahandle].noTime = 0;
	audiodevices[pahandle].captureStartTime = 0;
	audiodevices[pahandle].startTime = 0.0;
	audiodevices[pahandle].reqStopTime = stopTime;
	audiodevices[pahandle].estStopTime = 0;
	audiodevices[pahandle].currentTime = 0;		
	audiodevices[pahandle].schedule_pos = 0;
	
	// Reset recorded samples counter:
	audiodevices[pahandle].recposition = 0;

	// Reset read samples counter: This will discard possibly not yet fetched data.
	audiodevices[pahandle].readposition = 0;

	// Reset play position:
	audiodevices[pahandle].playposition = 0;
	
	// Reset total count of played out samples:
	audiodevices[pahandle].totalplaycount = 0;

	// Set number of requested repetitions: 0 means loop forever, default is 1 time.
	audiodevices[pahandle].repeatCount = (repetitions == 0) ? -1 : repetitions;

	// Reset any pending requests:
	audiodevices[pahandle].reqstate = 255;

	// Setup target start time:
	audiodevices[pahandle].reqStartTime = when;

	// Mark state as "hot-started":
	audiodevices[pahandle].state = 1;

	// Engine running?
	if (!Pa_IsStreamActive(audiodevices[pahandle].stream) || Pa_IsStreamStopped(audiodevices[pahandle].stream)) {
		// Try to start stream if the engine isn't running, either because it is the very
		// first call to 'Start' in any runMode, or because the engine got stopped in
		// preparation for a restart in runMode zero. Need to drop the lock during
		// call to Pa_StartStream to avoid deadlock:
		PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);

		// Safeguard: If the stream is not stopped, do it now:
		if (!Pa_IsStreamStopped(audiodevices[pahandle].stream)) Pa_StopStream(audiodevices[pahandle].stream);

		// Start engine:
		if ((err=Pa_StartStream(audiodevices[pahandle].stream))!=paNoError) {
			printf("PTB-ERROR: Failed to start audio device %i. PortAudio reports this error: %s \n", pahandle, Pa_GetErrorText(err));
			PsychErrorExitMsg(PsychError_system, "Failed to start PortAudio audio device.");
		}

		// Reacquire lock:
		PsychPALockDeviceMutex(&audiodevices[pahandle]);
	}
	
	// From here on, the engine is running, and we have the mutex-lock:
	
	// Wait for real start of playback/capture?
	if (waitForStart > 0) {
		// Device will be in state == 1 until playback really starts:
		// We need to enter the first while() loop iteration with
		// the device lock held from above, so the while() loop will iterate at
		// least once...
		while (audiodevices[pahandle].state == 1 && Pa_IsStreamActive(audiodevices[pahandle].stream)) {
			// Wait for a state-change before reevaluating the .state:
			PsychPAWaitForChange(&audiodevices[pahandle]);
		}
		
		// Device has started (potentially even already finished for very short sounds!)
		// In any case we have a valid .startTime to wait for in the device struct,
		// which won't change by itself anymore, so it is safe to access unlocked.
		// Unlock device:
		PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);
		
		// Ok, relevant audio buffer with real sound onset submit to engine.
		// We now have an estimate of real sound onset in startTime, wait until
		// then:
		PsychWaitUntilSeconds(audiodevices[pahandle].startTime);
		
		// Engine should run now. Return real onset time:
		PsychCopyOutDoubleArg(1, kPsychArgOptional, audiodevices[pahandle].startTime);
	}
	else {
		// Unlock device:
		PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);

		// Return empty zero timestamp to signal that this info is not available:
		PsychCopyOutDoubleArg(1, kPsychArgOptional, 0.0);
	}

	return(PsychError_none);
}

/* PsychPortAudio('Stop') - Stop an audio device via PortAudio.
 */
PsychError PSYCHPORTAUDIOStopAudioDevice(void) 
{
 	static char useString[] = "[startTime endPositionSecs xruns estStopTime] = PsychPortAudio('Stop', pahandle [, waitForEndOfPlayback=0] [, blockUntilStopped=1] [, repetitions] [, stopTime]);";
	static char synopsisString[] = 
		"Stop a PortAudio audio device. The 'pahandle' is the handle of the device to stop.\n"
		"'waitForEndOfPlayback' - If set to 1, this method will wait until playback of the "
		"audio stream finishes by itself. This only makes sense if you perform playback with "
		"a defined playback duration. The flag will be ignored when infinite repetition is "
		"requested (as playback would never stop by itself, resulting in a hang) and if no "
		"scheduled 'stopTime' has been set, either in this call or in the 'Start' function. "
		"The setting will be also ignored if this is a pure recording session.\n"
		"A setting of 0 (which is the default) requests stop of playback without waiting for all "
		"the sound to be finished playing. Sound may continue to be played back for multiple "
		"milliseconds after this call, as this is a polite request to the hardware to finish up.\n"
		"A setting of 2 requests abortion of playback and/or capture as soon as possible with your "
		"sound hardware, even if this creates audible artifacts etc. Abortion may or may not be faster "
		"than a normal stop, this depends on your specific hardware, but our driver tries as hard as "
		"possible to get the hardware to shut up. In a worst-case setting, the hardware would continue "
		"to playback the sound data that is stored in its internal buffers, so the latency for stopping "
		"sound would be the the same as the latency for starting sound as quickly as possible. E.g., "
		"a 2nd generation Intel MacBook Pro seems to have a stop-delay of roughly 5-6 msecs under "
		"optimal conditions (e.g., buffersize = 64, frequency=96000, OS/X 10.4.10, no other sound apps running).\n"
		"A setting of 3 will not try to stop the playback now: You can use this setting if you just "
		"want to use this function to wait until playback stops by itself, e.g. because the set "
		"number of 'repetitions' or the set 'stopTime' has been reached. You can also use this to "
		"just change the 'repetitions' or 'stopTime' settings without waiting for anything.\n"
		"The optional parameter 'blockUntilStopped' defines if the subfunction should wait until "
		"sound processing has really stopped (at a setting of 1, which is the default), or if the "
		"function should return with minimal delay after only scheduling a stop at a zero setting. "
		"If 'waitForEndOfPlayback' is set to 1, then 'blockUntilStopped' is meaningless and the function "
		"will always block until the stop is completed.\n"
		"The optional parameter 'stopTime' allows to set a defined system time when playback should "
		"stop by itself. Similar, the optional 'repetitions' setting allows to change the number of "
		"repetitions after which playback should stop by itself. These settings allow you to override "
		"the same settings made during a call to the 'Start' or 'RescheduleStart' function.\n"
		"The optional return argument 'startTime' returns an estimate of when the stopped "
		"stream actually started its playback and/or recording. Its the same timestamp as the one "
		"returned by the start command when executed in waiting mode. 'endPositionSecs' is the final "
		"playback/recording position in seconds. 'xruns' is the number of buffer over- or underruns. "
		"This should be zero if the playback operation was glitch-free, however a zero value doesn't "
		"imply glitch free operation, as the glitch detection algorithm can miss some types of glitches. "
		"The optional return argument 'estStopTime' returns an estimate of when playback likely stopped.\n"
		"The return arguments are undefined if you set the 'blockUntilStopped' flag to zero.\n";
	
	static char seeAlsoString[] = "Open GetDeviceSettings ";	 
	
	PaError err;
	int pahandle= -1;
	int waitforend = 0;
	int blockUntilStopped = 1;
	double stopTime = -1;
	double repetitions = -1;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(5));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(4));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");

	// Get optional wait-flag:
	PsychCopyInIntegerArg(2, kPsychArgOptional, &waitforend);

	// Get optional blockUntilStopped-flag:
	PsychCopyInIntegerArg(3, kPsychArgOptional, &blockUntilStopped);

	// Get new optional 'repetitions' count:
	if (PsychCopyInDoubleArg(4, kPsychArgOptional, &repetitions)) {
		// Argument provided: Range-Check and assign it:
		if (repetitions < 0) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'repetitions'. Valid values are zero or greater.");
	}
	else {
		repetitions = -1;
	}

	// Get optional stopTime:
	if (PsychCopyInDoubleArg(5, kPsychArgOptional, &stopTime)) {
		if ((stopTime <= audiodevices[pahandle].reqStartTime) && (stopTime < DBL_MAX)) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'stopTime'. Valid values are greater than previously set 'when' starttime.");
	}
	else {
		stopTime = -1;
	}

	// Lock device:
	PsychPALockDeviceMutex(&audiodevices[pahandle]);

	// New repetitions provided?
	if (repetitions >=0) {
		// Set number of requested repetitions: 0 means loop forever, default is 1 time.
		audiodevices[pahandle].repeatCount = (repetitions == 0) ? -1 : repetitions;
	}
	
	// New stopTime provided?
	if (stopTime > 0) {
		// Yes. Quickly assign it:
		audiodevices[pahandle].reqStopTime = stopTime;
	}
	
	// Wait for automatic stop of playback if requested: This only makes sense if we
	// are in playback mode and not in infinite playback mode! Would not make sense in
	// looping mode (infinite repetitions of playback) or pure recording mode. It is also
	// allowed if we have infinite repetitions set, but a finite stopTime is defined, so
	// the engine will eventually stop by itself. Same goes for an operative schedule which
	// will run empty if not regularly updated:
	if ((waitforend == 1) && Pa_IsStreamActive(audiodevices[pahandle].stream) && (audiodevices[pahandle].state > 0) &&
		(audiodevices[pahandle].opmode & kPortAudioPlayBack) && ((audiodevices[pahandle].repeatCount != -1) || (audiodevices[pahandle].schedule) || (audiodevices[pahandle].reqStopTime < DBL_MAX))) {
		while ( ((audiodevices[pahandle].runMode == 0) && Pa_IsStreamActive(audiodevices[pahandle].stream) && (audiodevices[pahandle].state > 0)) ||
				((audiodevices[pahandle].runMode == 1) && (audiodevices[pahandle].state > 0))) {

			// Wait for a state-change before reevaluating:
			PsychPAWaitForChange(&audiodevices[pahandle]);
		}
	}
	
	// Lock held here in any case...
	
	if (waitforend == 3) {
		// No immediate stop request: This was only either a query for end of playback,
		// or a call to simply set new 'stopTime' or 'repetitions' parameters on the fly.
		// Unlock the device and skip stop requests...
		PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);
	}
	else {
		// Some real immediate stop request wanted:
		// Soft stop requested (as opposed to fast stop)?
		if (waitforend!=2) {
			// Softstop: Try to stop stream:
			if (audiodevices[pahandle].state > 0) {
				// Stream running. Request a stop of stream, to be honored by playback thread:
				audiodevices[pahandle].reqstate = 0;
			}
				
			// Drop lock, so request can get through...
			PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);
				
			// If blockUntilStopped is non-zero, then explicitely stop as well:
			if ((blockUntilStopped > 0) && (audiodevices[pahandle].runMode == 0) && (!Pa_IsStreamStopped(audiodevices[pahandle].stream)) && (err=Pa_StopStream(audiodevices[pahandle].stream))!=paNoError) {
				printf("PTB-ERROR: Failed to stop audio device %i. PortAudio reports this error: %s \n", pahandle, Pa_GetErrorText(err));
				PsychErrorExitMsg(PsychError_system, "Failed to stop PortAudio audio device.");
			}
		}
		else {
			// Faststop: Try to abort stream. Skip if already stopped/not yet started:
			
			// Stream active?
			if (audiodevices[pahandle].state > 0) {
				// Yes. Set the 'state' flag to signal our IO-Thread not to push any audio
				// data anymore, but only zeros for silence and to paAbort asap:
				audiodevices[pahandle].reqstate = 3;
			}
			
			// Drop lock, so request can get through...
			PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);
			
			// If blockUntilStopped is non-zero, then send abort request to hardware:
			if ((blockUntilStopped > 0) && (audiodevices[pahandle].runMode == 0) && (!Pa_IsStreamStopped(audiodevices[pahandle].stream)) && ((err=Pa_AbortStream(audiodevices[pahandle].stream))!=paNoError)) {
				printf("PTB-ERROR: Failed to abort audio device %i. PortAudio reports this error: %s \n", pahandle, Pa_GetErrorText(err));
				PsychErrorExitMsg(PsychError_system, "Failed to fast stop (abort) PortAudio audio device.");
			}
		}
	}
	
	// No lock held here...

	// Wait for real stop:
	if (blockUntilStopped > 0) {
		// Lock device:
		PsychPALockDeviceMutex(&audiodevices[pahandle]);

		// Wait for stop / idle:
		if (Pa_IsStreamActive(audiodevices[pahandle].stream)) {
			while ( ((audiodevices[pahandle].runMode == 0) && Pa_IsStreamActive(audiodevices[pahandle].stream) && (audiodevices[pahandle].state > 0)) ||
					((audiodevices[pahandle].runMode == 1) && (audiodevices[pahandle].state > 0))) {
				
				// Wait for a state-change before reevaluating:
				PsychPAWaitForChange(&audiodevices[pahandle]);
			}
		}
		
		// We are stopped/idle, with lock held:
		
		// Need to update stream state and reqstate manually here, as the Pa_Stop/AbortStream()
		// requests may have stopped the paCallback() thread before it could update/honor state/reqstate by himself.

		// Mark state as stopped:
		audiodevices[pahandle].state = 0;
		
		// Reset request to none:
		audiodevices[pahandle].reqstate = 255;

		// Can unlock here, as the fields we're interested in will remain static with an idle/stopped engine:
		PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);

		// Copy out our estimate of when playback really started for the just stopped stream:
		PsychCopyOutDoubleArg(1, kPsychArgOptional, audiodevices[pahandle].startTime);
		
		// Copy out final playback position (secs) since start:
		PsychCopyOutDoubleArg(2, kPsychArgOptional, ((double)(audiodevices[pahandle].playposition / audiodevices[pahandle].outchannels)) / (double) audiodevices[pahandle].streaminfo->sampleRate);
		
		// Copy out number of buffer over-/underflows since start:
		PsychCopyOutDoubleArg(3, kPsychArgOptional, audiodevices[pahandle].xruns);

		// Copy out estimated stopTime:
		PsychCopyOutDoubleArg(4, kPsychArgOptional, audiodevices[pahandle].estStopTime);
	}
	else {
		// No block until stopped. That means we won't have meaningful return arguments available.
		// Just return dummy args to signal this:
		PsychCopyOutDoubleArg(1, kPsychArgOptional, -1);
		
		// Copy out final playback position (secs) since start:
		PsychCopyOutDoubleArg(2, kPsychArgOptional, -1);
		
		// Copy out number of buffer over-/underflows since start:
		PsychCopyOutDoubleArg(3, kPsychArgOptional, -1);

		// Copy out estimated stopTime:
		PsychCopyOutDoubleArg(4, kPsychArgOptional, -1);
	}

	return(PsychError_none);
}

/* PsychPortAudio('GetStatus') - Return current status of stream.
 */
PsychError PSYCHPORTAUDIOGetStatus(void) 
{
 	static char useString[] = "status = PsychPortAudio('GetStatus', pahandle);";
	static char synopsisString[] = 
		"Returns 'status', a struct with status information about the current state of device 'pahandle'.\n"
		"The struct contains the following fields:\n"
		"Active: Can be 1 if playback or recording is active, or 0 if playback/recording is stopped or not yet started.\n"
		"RequestedStartTime: Is the requested start time of the audio stream after start of playback/recording. "
		"StartTime: Is the real start time of the audio stream after start of playback/recording. If both, playback and "
		"recording are active (full-duplex mode), it is the start time of sound playback, ie an estimate of when the first "
		"sample hit the speakers. Same goes for pure playback. \n"
		"CaptureStartTime: Start time of audio capture (if any is active) - an estimate of when the first sound sample was "
		"captured. In pure capture mode, this is nearly identical to StartTime, but whenever playback is active, StartTime and "
		"CaptureStartTime will differ. CaptureStartTime doesn't take the user provided 'LatencyBias' into account.\n"
		"RequestedStopTime: The requested stop / sound offset time for playback of sounds, as selected via the 'stopTime' "
		"paramter in the 'Start', 'RescheduleStart' or 'Stop' function. This will show a very large (~ infinite) value "
		"if no stop time has been sprecified.\n"
		"EstimatedStopTime: Estimated time when sound playback has stopped. This is an estimate of when exactly the last "
		"audio sample will leave the speaker. The value is zero as long as the estimate isn't available. Due to the latency "
		"involved in sound playback, the value may become available a few msecs before or after actual sound offset.\n"
		"CurrentStreamTime: Estimate of when the most recently submitted sample will hit the speaker. This corresponds "
		"roughly to 'PositionSecs' below, but in absolute realtime.\n"
		"ElapsedOutSamples: Total number of samples played out since start of playback. This count increments monotonically "
		"from start of playback to stop of playback. This denotes the absolute sample position that will hit the speaker "
		"at time 'CurrentStreamTime'. \n"
		"PositionSecs is an estimate of the current stream playback position in seconds within the current playback loop "
		"of the current buffer. it's not totally accurate, because "
		"it measures how much sound has been submitted to the sound system, not how much sound has left the "
		"speakers, i.e., it doesn't take driver and hardware latency into account.\n"
		"SchedulePosition: Current position in a running schedule, if any.\n"
		"XRuns: Number of dropouts due to buffer overrun or underrun conditions.\n"
		"TotalCalls, TimeFailed and BufferSize are only for debugging of PsychPortAudio itself.\n"
		"CPULoad: How much load does the playback engine impose on the CPU? Values can range from 0.0 = 0% "
		"to 1.0 for 100%. Values close to 1.0 indicate that your system can't handle the load and timing glitches "
		"or sound glitches are likely. In such a case, try to reduce the load on your system.\n"
		"PredictedLatency: Is the latency in seconds of your driver+hardware combo. It tells you, "
		"how far ahead of time a sound device must be started ahead of the requested onset time via "
		"PsychPortAudio('Start'...) to make sure it actually starts playing in time. High quality systems like "
		"Linux or MacOS/X may allow values as low as 5 msecs or less on standard hardware. Other operating "
		"systems may require dozens or hundreds of milliseconds of headstart. Caution: In full-duplex mode, "
		"this value only refers to the latency on the sound output, not in the sound input! Also, this is just "
		"an estimate, not 100% reliable.\n"
		"LatencyBias: Is an additional bias setting you can impose via PsychPortAudio('LatencyBias', pahandle, bias); "
		"in case our drivers estimate is a bit off. Allows fine-tuning.\n"
		"SampleRate: Is the sampling rate for playback/recording in samples per second (Hz).\n"
		"RecordedSecs: Is the total amount of recorded sound data (in seconds) since start of capture.\n"
		"ReadSecs: Is the total amount of sound data (in seconds) that has been fetched from the internal buffer. "
		"The difference between RecordedSecs and ReadSecs is the amount of recorded sound data pending for retrieval. ";

	static char seeAlsoString[] = "Open GetDeviceSettings ";	 
	PsychGenericScriptType 	*status;
	double currentTime;
	unsigned int playposition, totalplaycount;

	const char *FieldNames[]={	"Active", "RequestedStartTime", "StartTime", "CaptureStartTime", "RequestedStopTime", "EstimatedStopTime", "CurrentStreamTime", "ElapsedOutSamples", "PositionSecs", "RecordedSecs", "ReadSecs", "SchedulePosition",
								"XRuns", "TotalCalls", "TimeFailed", "BufferSize", "CPULoad", "PredictedLatency", "LatencyBias", "SampleRate" };
	int pahandle = -1;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(1));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");

	PsychAllocOutStructArray(1, kPsychArgOptional, 1, 20, FieldNames, &status);

	// Ok, in a perfect world we should hold the device mutex while querying all the device state.
	// However, we don't: This reduces lock contention at the price of a small chance that the
	// fetched information is not 100% up to date / that this is not an atomic snapshot of state.
	//
	// Instead we only hold the lock to get the most crucial values atomically, then release and get the rest
	// while not holding the lock:
	PsychPALockDeviceMutex(&audiodevices[pahandle]);
	currentTime = audiodevices[pahandle].currentTime;
	totalplaycount = audiodevices[pahandle].totalplaycount;
	playposition = audiodevices[pahandle].playposition;
	PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);
	
	// Atomic snapshot for remaining fields would only be needed for low-level debugging, so who cares?
	PsychSetStructArrayDoubleElement("Active", 0, (audiodevices[pahandle].state >= 2) ? 1 : 0, status);
	PsychSetStructArrayDoubleElement("RequestedStartTime", 0, audiodevices[pahandle].reqStartTime, status);
	PsychSetStructArrayDoubleElement("StartTime", 0, audiodevices[pahandle].startTime, status);
	PsychSetStructArrayDoubleElement("CaptureStartTime", 0, audiodevices[pahandle].captureStartTime, status);
	PsychSetStructArrayDoubleElement("RequestedStopTime", 0, audiodevices[pahandle].reqStopTime, status);
	PsychSetStructArrayDoubleElement("EstimatedStopTime", 0, audiodevices[pahandle].estStopTime, status);
	PsychSetStructArrayDoubleElement("CurrentStreamTime", 0, currentTime, status);	
	PsychSetStructArrayDoubleElement("ElapsedOutSamples", 0, ((double)(totalplaycount / audiodevices[pahandle].outchannels)), status);
	PsychSetStructArrayDoubleElement("PositionSecs", 0, ((double)(playposition / audiodevices[pahandle].outchannels)) / (double) audiodevices[pahandle].streaminfo->sampleRate, status);
	PsychSetStructArrayDoubleElement("RecordedSecs", 0, ((double)(audiodevices[pahandle].recposition / audiodevices[pahandle].inchannels)) / (double) audiodevices[pahandle].streaminfo->sampleRate, status);
	PsychSetStructArrayDoubleElement("ReadSecs", 0, ((double)(audiodevices[pahandle].readposition / audiodevices[pahandle].inchannels)) / (double) audiodevices[pahandle].streaminfo->sampleRate, status);
	PsychSetStructArrayDoubleElement("SchedulePosition", 0, audiodevices[pahandle].schedule_pos, status);
	PsychSetStructArrayDoubleElement("XRuns", 0, audiodevices[pahandle].xruns, status);
	PsychSetStructArrayDoubleElement("TotalCalls", 0, audiodevices[pahandle].paCalls, status);
	PsychSetStructArrayDoubleElement("TimeFailed", 0, audiodevices[pahandle].noTime, status);
	PsychSetStructArrayDoubleElement("BufferSize", 0, audiodevices[pahandle].batchsize, status);
	PsychSetStructArrayDoubleElement("CPULoad", 0, (Pa_IsStreamActive(audiodevices[pahandle].stream)) ? Pa_GetStreamCpuLoad(audiodevices[pahandle].stream) : 0.0, status);
	PsychSetStructArrayDoubleElement("PredictedLatency", 0, audiodevices[pahandle].predictedLatency, status);
	PsychSetStructArrayDoubleElement("LatencyBias", 0, audiodevices[pahandle].latencyBias, status);
	PsychSetStructArrayDoubleElement("SampleRate", 0, audiodevices[pahandle].streaminfo->sampleRate, status);
	return(PsychError_none);
}

/* PsychPortAudio('Verbosity') - Set level of verbosity.
 */
PsychError PSYCHPORTAUDIOVerbosity(void) 
{
 	static char useString[] = "oldlevel = PsychPortAudio('Verbosity' [,level]);";
	static char synopsisString[] = 
		"Set level of verbosity for error/warning/status messages. 'level' optional, new level "
		"of verbosity. 'oldlevel' is the old level of verbosity. The following levels are "
		"supported: 0 = Shut up. 1 = Print errors, 2 = Print also warnings, 3 = Print also some info, "
		"4 = Print more useful info (default), >5 = Be very verbose (mostly for debugging the driver itself). ";		
	static char seeAlsoString[] = "Open GetDeviceSettings ";	 
	
	int level= -1;

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(1));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	PsychCopyInIntegerArg(1, kPsychArgOptional, &level);
	if (level < -1) PsychErrorExitMsg(PsychError_user, "Invalid level of verbosity provided. Valid are levels of zero and greater.");
	
	// Return current/old level:
	PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) verbosity);

	// Set new level, if one was provided:
	if (level > -1) verbosity = level;

	return(PsychError_none);
}

/* PsychPortAudio('GetOpenDeviceCount') - Get number of open audio devices.
 */
PsychError PSYCHPORTAUDIOGetOpenDeviceCount(void) 
{
 	static char useString[] = "count = PsychPortAudio('GetOpenDeviceCount');";
	static char synopsisString[] = "Return the number of currently open audio devices.\n";
	static char seeAlsoString[] = "Open ";	 

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(0));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	// Return count:
	PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) audiodevicecount);

	return(PsychError_none);
}

/* PsychPortAudio('LatencyBias') - Set a manual bias for the latencies we operate on.
 */
PsychError PSYCHPORTAUDIOLatencyBias(void) 
{
 	static char useString[] = "oldbias = PsychPortAudio('LatencyBias', pahandle [,biasSecs]);";
	static char synopsisString[] = 
		"Set audio output latency bias in seconds to 'biasSecs' and/or return old bias for a device "
		"'pahandle'. The device must be open for this setting to take effect. It is reset to zero at "
		"each reopening of the device. PsychPortAudio computes a latency value for the expected latency "
		"of an audio output device to get its timing right. If this latency value is slightly off for "
		"some reason, you can provide a bias value with this function to correct the computed value. "
		"See the online help for PsychPortAudio for more in depth explanation of latencies. ";		
	static char seeAlsoString[] = "Open GetDeviceSettings ";	 
	
	double bias= DBL_MAX;
	int pahandle = -1;

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(2));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");
	if (Pa_IsStreamActive(audiodevices[pahandle].stream) && (audiodevices[pahandle].state > 0)) PsychErrorExitMsg(PsychError_user, "Tried to change 'biasSecs' while device is active! Forbidden!");

	// Copy in optional new bias value:
	PsychCopyInDoubleArg(2, kPsychArgOptional, &bias);
	
	// Return current/old bias:
	PsychCopyOutDoubleArg(1, kPsychArgOptional, audiodevices[pahandle].latencyBias);

	// Set new bias, if one was provided:
	if (bias!=DBL_MAX) audiodevices[pahandle].latencyBias = bias;
	
	return(PsychError_none);
}

/* PsychPortAudio('GetDevices') - Enumerate all available sound devices.
 */
PsychError PSYCHPORTAUDIOGetDevices(void) 
{
 	static char useString[] = "devices = PsychPortAudio('GetDevices' [,devicetype]);";
	static char synopsisString[] = 
		"Returns 'devices', an array of structs, one struct for each available PortAudio device. "
		"Each struct contains information about its associated PortAudio device. The optional "
		"parameter 'devicetype' can be used to enumerate only devices of a specific class: \n"
		"1=Windows/DirectSound, 2=Windows/MME, 3=Windows/ASIO, 11=Windows/WDMKS, 13=Windows/WASAPI, "
		"8=Linux/ALSA, 7=Linux/OSS, 12=Linux/JACK, 5=MacOSX/CoreAudio.\n "
		"On OS/X you'll usually only see devices for the CoreAudio API, a first-class audio subsystem. "
		"On Linux you may have the choice between ALSA, JACK and OSS. ALSA or JACK provide very low "
		"latencies and very good timing, OSS is an older system which is less capable but not very "
		"widespread in use anymore. On MS-Windows you'll have the ''choice'' between up to 5 different "
		"audio subsystems: If you buy an expensive sound card with ASIO drivers, pick that API for low "
		"latency, it should give you comparable performance to OS/X or Linux. 2nd best choice "
		"would be WASAPI (on Windows-Vista) or WDMKS (on Windows-2000/XP) for ok latency on good days. DirectSound is the next "
		"worst choice if you have hardware with DirectSound support. If everything else fails, you'll be left "
		"with MMS, a premium example of system misdesign successfully sold to paying customers.";
		
	static char seeAlsoString[] = "Open GetDeviceSettings ";	 
	PsychGenericScriptType 	*devices;
	const char *FieldNames[]={	"DeviceIndex", "HostAudioAPIId", "HostAudioAPIName", "DeviceName", "NrInputChannels", "NrOutputChannels", 
								 		"LowInputLatency", "HighInputLatency", "LowOutputLatency", "HighOutputLatency",  "DefaultSampleRate", "xxx" };
	int devicetype = -1;
	int count = 0;
	int i, ic, filteredcount;
	PaDeviceInfo* padev = NULL;
	PaHostApiInfo* hainfo = NULL;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(1));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgOptional, &devicetype);
	if (devicetype < -1) PsychErrorExitMsg(PsychError_user, "Invalid 'devicetype' provided. Valid are levels of zero and greater.");
	
	// Query number of devices and allocate out struct array:
	count = (int) Pa_GetDeviceCount();
	if (count > 0) {
		filteredcount = count;
		if (devicetype!=-1) {
			filteredcount = 0;
			// Filtering bz host API requested: Do it.
			for (i=0; i<count; i++) {
				padev = Pa_GetDeviceInfo((PaDeviceIndex) i);
				hainfo = Pa_GetHostApiInfo(padev->hostApi);
				if (hainfo->type == devicetype) filteredcount++;
			}
		}

		PsychAllocOutStructArray(1, kPsychArgOptional, filteredcount, 11, FieldNames, &devices);
	}
	else {
		PsychErrorExitMsg(PsychError_user, "PTB-ERROR: PortAudio can't detect any supported sound device on this system.");
	}
	
	// Iterate through device list:
	ic = 0;
	for (i=0; i<count; i++) {
		padev = Pa_GetDeviceInfo((PaDeviceIndex) i);
		hainfo = Pa_GetHostApiInfo(padev->hostApi);
		if ((devicetype==-1) || (hainfo->type == devicetype)) {
			PsychSetStructArrayDoubleElement("DeviceIndex", ic, i, devices);
			PsychSetStructArrayDoubleElement("HostAudioAPIId", ic, hainfo->type, devices);
			PsychSetStructArrayStringElement("HostAudioAPIName", ic, hainfo->name, devices);
			PsychSetStructArrayStringElement("DeviceName", ic, padev->name, devices);
			PsychSetStructArrayDoubleElement("NrInputChannels", ic, padev->maxInputChannels, devices);
			PsychSetStructArrayDoubleElement("NrOutputChannels", ic, padev->maxOutputChannels, devices);
			PsychSetStructArrayDoubleElement("LowInputLatency", ic, padev->defaultLowInputLatency, devices);
			PsychSetStructArrayDoubleElement("HighInputLatency", ic, padev->defaultHighInputLatency, devices);
			PsychSetStructArrayDoubleElement("LowOutputLatency", ic, padev->defaultLowOutputLatency, devices);
			PsychSetStructArrayDoubleElement("HighOutputLatency", ic, padev->defaultHighOutputLatency, devices);
			PsychSetStructArrayDoubleElement("DefaultSampleRate", ic, padev->defaultSampleRate, devices);
			// PsychSetStructArrayDoubleElement("xxx", ic, 0, devices);
			ic++;
		}
	}
	
	return(PsychError_none);
}

/* PsychPortAudio('RunMode') - Select a different mode of operation.
 */
PsychError PSYCHPORTAUDIORunMode(void) 
{
 	static char useString[] = "oldRunMode = PsychPortAudio('RunMode', pahandle [,runMode]);";
	static char synopsisString[] = 
		"Set general run mode to 'runMode' and/or return old runMode for a device 'pahandle'.\n"
		"The device must be open for this setting to take effect and playback must be stopped if "
		"one wants to change the setting. If playback isn't stopped, it will be forcefully stopped. "
		"The current/old runMode is returned in the optional return value 'oldRunMode'. "
		"'runMode' is the optional new runmode: At device open time, the runMode defaults to zero. "
		"In mode zero, the audio hardware and all internal processing are completely stopped at end "
		"of audio playback. This reduces system ressource usage (both hardware and computation time), "
		"but may cause slightly longer latencies when re'Start'ing the device for playback of a different "
		"sound, e.g., a sequence of sounds. In 'runMode' 1, the audio hardware and processing don't shut "
		"down at the end of audio playback. Instead, everything remains active in a ''hot standby'' state. "
		"This allows to very quickly (with low latency) restart sound playback via the 'RescheduleStart' "
		"function. The downside is a permanent use of system ressources even if no sound is playing. "
		"Future runMode settings may provide more interesting options, stay tuned...\n";
		
	static char seeAlsoString[] = "Start Stop RescheduleStart ";
	
	int runMode = -1;
	int pahandle = -1;

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(2));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");

	// Copy in optional runMode value:
	PsychCopyInIntegerArg(2, kPsychArgOptional, &runMode);
	
	// Return current/old runMode:
	PsychCopyOutDoubleArg(1, kPsychArgOptional, audiodevices[pahandle].runMode);

	// Set new runMode, if one was provided:
	if (runMode != -1) {
		// Stop engine if it is running:
		if (!Pa_IsStreamStopped(audiodevices[pahandle].stream)) Pa_StopStream(audiodevices[pahandle].stream);

		// Reset state:
		audiodevices[pahandle].state = 0;
		audiodevices[pahandle].reqstate = 255;

		if (runMode < 0 || runMode > 1) PsychErrorExitMsg(PsychError_user, "Invalid 'runMode' provided. Must be 0 or 1!");

		// Assign new runMode:
		audiodevices[pahandle].runMode = runMode;
	}

	return(PsychError_none);
}

/* PsychPortAudio('SetLoop') - Define a subrange of samples in the audio playback buffer to play.
 */
PsychError PSYCHPORTAUDIOSetLoop(void) 
{
 	static char useString[] = "PsychPortAudio('SetLoop', pahandle[, startSample=0][, endSample=max][, UnitIsSeconds=0]);";
	static char synopsisString[] = 
		"Restrict audio playback to a subrange of sound samples in the current audio playback buffer for "
		"audio device 'pahandle'. The device must be open and a soundbuffer created via 'FillBuffer' "
		"for this setting to take effect. The setting is reset at each non-streaming 'FillBuffer' call.\n"
		"'startSample' defines the first sample to be played, it defaults to zero, ie. the very first in "
		"the buffer. 'endSample' the last sample to be played, it defaults to the end of the buffer.\n"
		"Calling this function without any parameter except 'pahandle' will therefore reset the playloop "
		"to the full buffer size. It is possible but not advisable to change the loop range while "
		"sound playback is active, as this may cause audible glitches or artifacts in the playback.\n"
		"By default, 'startSample' and 'endSample' are specified in units of samples, but if the "
		"optional flag 'UnitIsSeconds' is set to a non-zero value, then the given range is "
		"interpreted in units of seconds. ";
		
	static char seeAlsoString[] = "FillBuffer Start Stop RescheduleStart ";
	
	double startSample, endSample, sMultiplier;
	int maxSample, unitIsSecs;
	int pahandle = -1;

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(4));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(0));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");

	unitIsSecs = 0;
	PsychCopyInIntegerArg(4, kPsychArgOptional, &unitIsSecs);
	sMultiplier = (unitIsSecs > 0) ? (double) audiodevices[pahandle].streaminfo->sampleRate : 1.0;

	// Compute maxSample the maximum possible sampleframe index for given soundbuffer:
	maxSample = (audiodevices[pahandle].outputbuffersize / sizeof(float) / audiodevices[pahandle].outchannels) - 1;

	// Copy in optional startSample:
	startSample = 0;
	PsychCopyInDoubleArg(2, kPsychArgOptional, &startSample);
	if (startSample < 0) PsychErrorExitMsg(PsychError_user, "Invalid 'startSample' provided. Must be greater or equal to zero!");
	startSample *= sMultiplier;
	
	// Copy in optional endSample:
	if (PsychCopyInDoubleArg(3, kPsychArgOptional, &endSample)) {
		endSample *= sMultiplier;
		if (endSample > maxSample) PsychErrorExitMsg(PsychError_user, "Invalid 'endSample' provided. Must be no greater than total buffersize!");
	}
	else {
		endSample = maxSample;
	}

	if (endSample < startSample) PsychErrorExitMsg(PsychError_user, "Invalid 'endSample' provided. Must be greater or equal than 'startSample'!");
	
	// Ok, range is valid. Assign it:
	PsychPALockDeviceMutex(&audiodevices[pahandle]);
	audiodevices[pahandle].loopStartFrame = (unsigned int) startSample;
	audiodevices[pahandle].loopEndFrame = (unsigned int) endSample;
	PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);

	return(PsychError_none);
}

/* PsychPortAudio('EngineTunables') - Set tunable low-level engine parameters
 */
PsychError PSYCHPORTAUDIOEngineTunables(void) 
{
 	static char useString[] = "[oldyieldInterval, oldMutexEnable, lockToCore1] = PsychPortAudio('EngineTunables' [, yieldInterval] [, MutexEnable] [, lockToCore1]);";
	static char synopsisString[] = 
		"Return, and optionally set low-level tuneable driver parameters.\n"
		"The driver must be idle, ie., no audio device must be open, if you want to change tuneables! "
		"These tuneable parameters usually have reasonably chosen defaults and you should only "
		"need to change them to work around bugs or flaws in your operating system, sound hardware or drivers, "
		"or if you have very unusual needs or setups. Only touch these if you know what you're doing, probably "
		"after consultation with the Psychtoolbox forum or Wiki. Some of these have potential to cause serious "
		"system malfunctions if not selected properly!\n\n"
		"'yieldInterval' - If the driver has to perform polling operations, it will release the cpu for "
		"yieldInterval seconds inbetween unsuccessful polling iterations. Valid range is 0.0 to 0.1 secs, with "
		"a reasonable default of 0.001 secs ie. 1 msec.\n"
		"'MutexEnable' - Enable (1) or Disable (0) internal mutex locking of driver data structures to prevent "
		"potential race-conditions between internal processing threads. Locking is enabled by default. Only "
		"disable locking to work around seriously broken audio device drivers or system setups and be aware "
		"that this may have unpleasant side effects and can cause all kinds of malfunctions by itself!\n"
		"'lockToCore1' - Enable (1) or Disable (0) locking of all audio engine processing threads to cpu core 1 "
		"on Microsoft Windows systems. By default threads are locked to cpu core 1 to avoid problems with "
		"timestamping due to bugs in some microprocessors clocks and in Microsoft Windows itself. If you're "
		"confident/certain that your system is bugfree wrt. to its clocks and want to get a bit more "
		"performance out of multi-core machines, you can disable this. You must perform this setting before "
		"you open the first audio device the first time, otherwise the setting might be ignored.\n";

	static char seeAlsoString[] = "Open ";	 
	
	int mutexenable, mylockToCore1;
	double myyieldInterval;

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(3));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(3));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	// Make sure no settings are changed while an audio device is open:
	if ((PsychGetNumInputArgs() > 0) && (audiodevicecount > 0)) PsychErrorExitMsg(PsychError_user, "Tried to change low-level engine parameter while at least one audio device is open! Forbidden!");

	// Return old yieldInterval:
	PsychCopyOutDoubleArg(1, kPsychArgOptional, yieldInterval);

	// Get optional new yieldInterval:
	if (PsychCopyInDoubleArg(1, kPsychArgOptional, &myyieldInterval)) {
		if (myyieldInterval < 0 || myyieldInterval > 0.1) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'yieldInterval' provided. Valid are between 0.0 and 0.1 seconds.");
		yieldInterval = myyieldInterval;
		if (verbosity > 3) printf("PsychPortAudio: INFO: Engine yieldInterval changed to %lf seconds.\n", yieldInterval);
	}

	// Return current/old mutexenable:
	PsychCopyOutDoubleArg(2, kPsychArgOptional, (double) ((uselocking) ? 1 : 0));

	// Get optional new mutexenable:
	if (PsychCopyInIntegerArg(2, kPsychArgOptional, &mutexenable)) {
		if (mutexenable < 0 || mutexenable > 1) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'MutexEnable' provided. Valid are 0 and 1.");
		uselocking = (mutexenable > 0) ? TRUE : FALSE;
		if (verbosity > 3) printf("PsychPortAudio: INFO: Engine Mutex locking %s.\n", (uselocking) ? "enabled" : "disabled");
	}

	// Return current/old lockToCore1:
	PsychCopyOutDoubleArg(3, kPsychArgOptional, (double) ((lockToCore1) ? 1 : 0));

	// Get optional new lockToCore1:
	if (PsychCopyInIntegerArg(3, kPsychArgOptional, &mylockToCore1)) {
		if (mylockToCore1 < 0 || mylockToCore1 > 1) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'lockToCore1' provided. Valid are 0 and 1.");
		lockToCore1 = (mylockToCore1 > 0) ? TRUE : FALSE;
		if (verbosity > 3) printf("PsychPortAudio: INFO: Locking of all engine threads to cpu core 1 %s.\n", (lockToCore1) ? "enabled" : "disabled");
	}


	return(PsychError_none);
}

/* PsychPortAudio('UseSchedule') - Enable & Create, or disable and destroy a playback schedule.
 */
PsychError PSYCHPORTAUDIOUseSchedule(void) 
{
 	static char useString[] = "PsychPortAudio('UseSchedule', pahandle, enableSchedule [, maxSize = 128]);";
	static char synopsisString[] = 
		"Enable or disable use of a preprogrammed schedule for audio playback on audio device 'pahandle'.\n"
		"Schedules are similar to playlists on your favorite audio player. A schedule allows to define a sequence "
		"of distinct sounds to play in succession. When PsychPortAudio('Start') is called, processing of "
		"the schedule begins and the first programmed sound snippet is played, followed by the 2nd, 3rd, ... "
		"until the whole schedule has been played once and playback stops. You can add new sound snippets to the "
		"schedule while playback is running for uninterrupted playback of long sequences of sound.\n"
		"Each sound snippet or slot in the schedule defines a soundbuffer to play back via a 'bufferhandle', "
		"within the buffer a subsegment (a so called playloop) defined by start- and endpoint, and a number "
		"of repetitions for that playloop.\n"
		"This subfunction allows to either enable use of schedules by setting the 'enableSchedule' flag to 1, "
		"in which case a schedule with a maximum of 'maxSize' distinct slots is created ('maxSize' defaults to 128 slots), "
		"or to disable use of schedules by setting 'enableSchedule' to 0, in which case PsychPortAudio reverts "
		"back to its normal playback behaviour and an existing schedule is deleted.\n"
		"A 'enableSchedule' setting of 2 will reset an existing schedule, ie. clear it of all its entries, "
		"so it is ready to be rewritten with new entries. You should reset and rewrite a schedule each "
		"time after playback/processing of a schedule has finished or has been stopped.\n"
		"A 'enableSchedule' setting of 3 will reactivate an existing schedule, ie. prepare it for a replay.\n"
		"See the subfunction 'AddToSchedule' on how to populate the schedule with actual entries.\n";
		
	static char seeAlsoString[] = "FillBuffer Start Stop RescheduleStart AddToSchedule";
	
	int pahandle = -1;
	int enableSchedule;
	int maxSize = 128;
	int j;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(3));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(2)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(0));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");

	// Make sure the device is fully idle: We can check without mutex held, as a device which is
	// already idle (state == 0) can't switch by itself out of idle state (state > 0), neither
	// can an inactive stream start itself.
	if ((audiodevices[pahandle].state > 0) && Pa_IsStreamActive(audiodevices[pahandle].stream)) PsychErrorExitMsg(PsychError_user, "Tried to enable/disable audio schedule while audio device is active. Forbidden! Call 'Stop' first.");

	// At this point the deivce is idle and will remain so during this routines execution,
	// so it won't touch any of the schedule related variables and we can manipulate them
	// without a need to lock the device:

	// Get required enable flag:
	PsychCopyInIntegerArg(2, kPsychArgRequired, &enableSchedule);
	if (enableSchedule < 0 || enableSchedule > 3)  PsychErrorExitMsg(PsychError_user, "Invalid 'enableSchedule' provided. Must be 0, 1, 2 or 3!");

	// Get the optional maxSize parameter:
	PsychCopyInIntegerArg(3, kPsychArgOptional, &maxSize);
	if (maxSize < 1) PsychErrorExitMsg(PsychError_user, "Invalid 'maxSize' provided. Must be greater than zero!");

	// Revival of existing schedule requested?
	if (enableSchedule == 3) {
		if (NULL == audiodevices[pahandle].schedule) {
			PsychErrorExitMsg(PsychError_user, "'enableSchedule' == 3 requested to revive current schedule, but no such schedule exists! You must create it first.");
		}

		// Reset current position in schedule to start:
		audiodevices[pahandle].schedule_pos = 0;
		
		for (j = 0; j < audiodevices[pahandle].schedule_size; j++) {
			// Slot occupied but inactive?
			if (audiodevices[pahandle].schedule[j].mode & 1) {
				// Reactivate this slot to pending:
				audiodevices[pahandle].schedule[j].mode = 2;
			}
		}
		
		// Done.
		return(PsychError_none);
	}


	// Reset of existing schedule requested?
	if ((enableSchedule == 2) && (audiodevices[pahandle].schedule)) {
		// Yes: Simply set requested size to current size, this will trigger
		// a memset() to zero below, instead of a realloc or alloc, which is
		// exactly what we want:
		maxSize = audiodevices[pahandle].schedule_size;
	}

	// Release an already existing schedule: This will take care of both,
	// disabling use of schedules if this is a disable call, and resetting
	// of an existing schedule if this is an enable call following another
	// enable call:
	if (audiodevices[pahandle].schedule) {
		// Schedule already exists: Is this by any chance an enable call and
		// the requested size of the new schedule matches the size of the current
		// one?
		if (enableSchedule && (audiodevices[pahandle].schedule_size == maxSize)) {
			// Yes! Have a schedule of exactly wanted size, no need to free and
			// realloc - We simply clear it out by zero filling:
			memset((void*) audiodevices[pahandle].schedule, 0, (size_t) (maxSize * sizeof(PsychPASchedule)));
		}
		else {
			// No. Release old schedule...
			free(audiodevices[pahandle].schedule);
			audiodevices[pahandle].schedule = NULL;
			audiodevices[pahandle].schedule_size = 0;
		}
	}

	// Reset current position in schedule to start and size to zero in any case:
	audiodevices[pahandle].schedule_pos = 0;
	audiodevices[pahandle].schedule_writepos = 0;
	
	// Enable/Reset request?
	if (enableSchedule && (NULL == audiodevices[pahandle].schedule)) {
		// Enable request - Allocate proper schedule:
		audiodevices[pahandle].schedule_size = 0;
		audiodevices[pahandle].schedule = (PsychPASchedule*) calloc(maxSize, sizeof(PsychPASchedule));
		if (audiodevices[pahandle].schedule == NULL) PsychErrorExitMsg(PsychError_outofMemory, "Insufficient free system memory when trying to create a schedule!");

		// Assign new size:
		audiodevices[pahandle].schedule_size = maxSize;
	}

	// Done.
	return(PsychError_none);
}

/* PsychPortAudio('AddToSchedule') - Add command slots to a playback schedule.
 */
PsychError PSYCHPORTAUDIOAddToSchedule(void) 
{
 	static char useString[] = "[success, freeslots] = PsychPortAudio('AddToSchedule', pahandle [, bufferHandle=0][, repetitions=1][, startSample=0][, endSample=max][, UnitIsSeconds=0]);";
	//																				  1           2                 3                4                5                6
	static char synopsisString[] = 
		"Add a new item to an existing schedule for audio playback on audio device 'pahandle'.\n"
		"The schedule must have been created and enabled already via a previous call to 'UseSchedule'. "
		"The function returns if the addition of a new item was successfull via the return argument "
		"'success' (1=Success, 0=Failed), and the number of remaining free slots in 'freeslots'. "
		"Failure to add an item can happen if the schedule is full. If playback is running, you can "
		"simply retry after some time, because eventually the playback will consume and thereby free "
		"at least one slot in the schedule. If playback is stopped and you get this failure, you should "
		"reallocate the schedule with a bigger size via a proper call to 'UseSchedule'.\n"
		"Please note that after playback/processing of a schedule has finished by itself, or due to "
		"'Stop'ping the playback via the stop function, you should clear or reactivate the schedule and rewrite "
		"it, otherwise results at next call to 'Start' may be undefined. You can clear/reactivate a schedule "
		"efficiently without resizing it by calling 'UseSchedule' with an enableFlag of 2 or 3.\n\n"
		"The following optional paramters can be used to define the new slot in the schedule:\n"
		"'bufferHandle' Handle of the audio buffer which should be used for playback of this slot. "
		"The default value zero will play back the standard audio buffer created by a call to 'FillBuffer'.\n"
		"'repetitions' How often should playback of this slot be repeated. Fractional positive values are "
		"allowed, the value zero (ie. infinite repetition) is not allowed in this driver release.\n"
		"'startSample' and 'endSample' define a playback loop - a subsegment of the audio buffer to which "
		"playback should be restricted, and 'UnitIsSeconds' tells if the given loop boundaries are expressed "
		"in audio sample frames, or in seconds realtime. See the help for the 'SetLoop' function for more "
		"explanation.\n";

	static char seeAlsoString[] = "FillBuffer Start Stop RescheduleStart UseSchedule";
	
	PsychPASchedule* slot;
	PsychPABuffer* buffer;
	int	slotid;
	double startSample, endSample, sMultiplier;
	int maxSample, unitIsSecs;
	int pahandle = -1;
	int bufferHandle = 0;
	double repetitions = 1;
	int success = 0;
	int freeslots = 0;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(6));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(2));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");

	// Make sure there is a schedule available:
	if (audiodevices[pahandle].schedule == NULL) PsychErrorExitMsg(PsychError_user, "You tried to AddToSchedule, but use of schedules is disabled! Call 'UseSchedule' first to enable them.");

	// Get optional bufferhandle:
	PsychCopyInIntegerArg(2, kPsychArgOptional, &bufferHandle);

	if (bufferHandle < 0) PsychErrorExitMsg(PsychError_user, "Invalid 'bufferHandle' provided. Must be greater or equal to zero, and a handle to an existing buffer!");

	// If it is a non-zero handle, try to dereference from dynamic buffer:
	if (bufferHandle > 0) {
		// Deref bufferHandle: Issue error if no buffer with such a handle exists:
		buffer = PsychPAGetAudioBuffer(bufferHandle);
		
		// Validate matching output channel count:
		if (buffer->outchannels != audiodevices[pahandle].outchannels) {
			printf("PsychPortAudio-ERROR: Audio channel count %i of audiobuffer with handle %i doesn't match channel count %i of audio device!\n", buffer->outchannels, bufferHandle, audiodevices[pahandle].outchannels);
			PsychErrorExitMsg(PsychError_user, "Referenced audio buffer 'bufferHandle' has an audio channel count that doesn't match channels of audio device!");
		}
	}

	// Get optional repetitions
	PsychCopyInDoubleArg(3, kPsychArgOptional, &repetitions);
	if (repetitions <= 0) PsychErrorExitMsg(PsychError_user, "Invalid 'repetitions' provided. Must be greater than zero!");

	// Get loop parameters, if any:
	unitIsSecs = 0;
	PsychCopyInIntegerArg(6, kPsychArgOptional, &unitIsSecs);
	sMultiplier = (unitIsSecs > 0) ? (double) audiodevices[pahandle].streaminfo->sampleRate : 1.0;

	// Set maxSample to maximum integer: The scheduler (aka PsychPAProcessSchedule()) will test at runtime if the playloop extends
	// beyond valid playbuffer boundaries and clamp to end-of-buffer if needed, so this is safe:
	maxSample = INT_MAX;

	// Copy in optional startSample:
	startSample = 0;
	PsychCopyInDoubleArg(4, kPsychArgOptional, &startSample);
	if (startSample < 0) PsychErrorExitMsg(PsychError_user, "Invalid 'startSample' provided. Must be greater or equal to zero!");
	startSample *= sMultiplier;
	
	// Copy in optional endSample:
	if (PsychCopyInDoubleArg(5, kPsychArgOptional, &endSample)) {
		endSample *= sMultiplier;
		if (endSample > maxSample) PsychErrorExitMsg(PsychError_user, "Invalid 'endSample' provided. Must be no greater than total buffersize!");
	}
	else {
		endSample = maxSample;
	}

	if (endSample < startSample) PsychErrorExitMsg(PsychError_user, "Invalid 'endSample' provided. Must be greater or equal than 'startSample'!");
	
	// All settings validated and ready to initialize a slot in the schedule:

	// Lock device:
	PsychPALockDeviceMutex(&audiodevices[pahandle]);

	// Map writepos to slotindex:
	slotid = audiodevices[pahandle].schedule_writepos % audiodevices[pahandle].schedule_size;
	
	// Enough unoccupied space in schedule? Ie., is this slot free (either never used, or already consumed and ready for recycling)?
	if ((audiodevices[pahandle].schedule[slotid].mode & 2) == 0) {
		// Fill slot:
		slot = (PsychPASchedule*) &(audiodevices[pahandle].schedule[slotid]);
		slot->mode = 2;
		slot->bufferhandle   = bufferHandle;
		slot->repetitions    = repetitions;
		slot->loopStartFrame = startSample;
		slot->loopEndFrame   = endSample;
		
		// Advance write position for next update iteration:
		audiodevices[pahandle].schedule_writepos++;
		
		// Recompute number of free slots:
		if (audiodevices[pahandle].schedule_size >= (audiodevices[pahandle].schedule_writepos - audiodevices[pahandle].schedule_pos)) {
			freeslots = audiodevices[pahandle].schedule_size - (audiodevices[pahandle].schedule_writepos - audiodevices[pahandle].schedule_pos);
		}
		else {
			freeslots = 0;
		}
		
		success = 1;
	}
	else {
		// Nope. No free slot:
		success = 0;
		freeslots = 0;
	}
	
	// Unlock device:
	PsychPAUnlockDeviceMutex(&audiodevices[pahandle]);

	// Return optional result code:
	PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) success);

	// Return optional remaining number of free slots:
	PsychCopyOutDoubleArg(2, kPsychArgOptional, (double) freeslots);
	
	return(PsychError_none);
}
