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
	
	This links statically against libportaudio.a on OS/X. The current universal
	binary version of libportaudio.a for OS/X can be found in the...
	PsychSourceGL/Cohorts/PortAudio/ subfolder, which also contains versions for
	Linux and Windows.
	
*/


#include "PsychPortAudio.h"

#if PSYCH_SYSTEM == PSYCH_OSX
#include "pa_mac_core.h"
#endif

#define MAX_SYNOPSIS_STRINGS 50  

//declare variables local to this file.  
static const char *synopsisSYNOPSIS[MAX_SYNOPSIS_STRINGS];

#define kPortAudioPlayBack 1
#define kPortAudioCapture  2
#define kPortAudioFullDuplex 3

// Maximum number of audio devices we handle:
#define MAX_PSYCH_AUDIO_DEVS 10

// Our device record:
typedef struct PsychPADevice {
	PaStream *stream;			// Pointer to associated portaudio stream.
	PaStreamInfo* streaminfo;   // Pointer to stream info structure, provided by PortAudio.
	PaHostApiTypeId hostAPI;	// Type of host API.
	double	 startTime;			// Requested playback start time in system time (secs). Returns real start time after start.
	int		 state;				// Current state of the stream: 0=Stopped, 1=Hot Standby, 2=Playing.
	int		 repeatCount;		// Number of repetitions: -1 = Loop forever, 1 = Once, n = n repetitions.
	float*	 buffer;			// Pointer to float memory buffer with sound output data.
	int		 buffersize;		// Size of buffer in bytes.
	unsigned int playposition;	// Current playposition in samples since start of playback (not frames, not bytes!)
	unsigned int channels;		// Number of output channels.
	unsigned int xruns;			// Number of over-/underflows of input-/output channel for this stream.
	unsigned int paCalls;		// Number of callback invocations.
	unsigned int noTime;		// Number of timestamp malfunction - Should not happen anymore.
	unsigned int batchsize;		// Maximum number of frames requested during callback invokation: Estimate of real buffersize.
	double	 predictedLatency;  // Latency that PortAudio predicts for current callbackinvocation. We will compensate for that when starting audio.
	double   latencyBias;		// A bias value to add to the value that PortAudio reports for total buffer->Speaker latency.
								// This value defaults to zero, but can be set up automatically on OS/X or manually on other OSes to compensate
								// for slight mistakes in PA's estimate.
} PsychPADevice;

PsychPADevice audiodevices[MAX_PSYCH_AUDIO_DEVS];
unsigned int  audiodevicecount;
unsigned int  verbosity = 4;

boolean pa_initialized = FALSE;

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
    unsigned long i, silenceframes;
	unsigned long channels;
	unsigned int  playposition, sbsize;
	double now, firstsampleonset, onsetDelta;
	int repeatCount;
	PaHostApiTypeId hA;

	// Sound output buffer attached to stream? If no sound buffer
	// is attached, we can't continue and tell the engine to abort
	// processing of this stream:
    if (dev == NULL || dev->buffer == NULL) return(paAbort);

	// Count total number of calls:
	dev->paCalls++;
	
	// Call number of timestamp failures:
	if (timeInfo->currentTime == 0) dev->noTime++;
	
	// Keep track of maximum number of frames requested:
	if (dev->batchsize < framesPerBuffer) dev->batchsize = framesPerBuffer;
	
	// Keep track of buffer over-/underflows:
	if (statusFlags & (paInputOverflow | paInputUnderflow | paOutputOverflow | paOutputUnderflow)) dev->xruns++;

	// Query number of output channels:
	channels = (unsigned long) dev->channels;
	
	// Query number of repetitions:
	repeatCount = dev->repeatCount;
	
	// Get our current playback position in samples (not frames or bytes!!):
	playposition = dev->playposition;

	// Compute size of soundbuffer in samples:
	sbsize = dev->buffersize / sizeof(float);

	// Logical playback state is "stopped"? If so, abort.
	if (dev->state == 0) {
		// Prime the buffer with silence to avoid ugly noise.
		memset(outputBuffer, 0, framesPerBuffer * channels * sizeof(float));
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
	// a zero filled buffer --> We output silence. If the deadline is somewhere
	// in the middle of this buffer, we fill the appropriate amount of bufferspace
	// with zeros, then copy in our first real samples into the remaining buffer.
	// After that, we switch to real playback, all future calls will provide PA
	// with real sampledata. Assuming the sound onset estimate provided by PA is
	// correct, this should allow accurate sound onset. It all depends on the
	// latency estimate...
	hA=dev->hostAPI;
	
	// Hot standby?
	if (dev->state == 1) {
		// Hot standby: Query and convert timestamps to system time.
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
			firstsampleonset = (double) timeInfo->outputBufferDacTime + dev->latencyBias;
			
			// Store our measured PortAudio + CoreAudio + Driver + Hardware latency:
			dev->predictedLatency = firstsampleonset - now;
		}
		else {
			// ASIO or unknown. ASIO needs to be checked, which category is correct.
			// Not yet verified how these other audio APIs behave. Play safe
			// and perform timebase remapping: This also needs our special fixed
			// PortAudio version where currentTime actually has a value:
			firstsampleonset = ((double) (timeInfo->outputBufferDacTime - timeInfo->currentTime));

			// Store our measured PortAudio + HostAPI + Driver + Hardware latency:
			dev->predictedLatency = firstsampleonset + dev->latencyBias;

			// Assign predicted (remapped to our time system) audio onset time for this buffer:
			firstsampleonset = now + firstsampleonset + dev->latencyBias;
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
		
		// Compute difference between requested onset time and presentation time
		// of the first sample of this callbacks returned buffer:
		onsetDelta = dev->startTime - firstsampleonset;
		
		// Time left until onset?
		if (onsetDelta > 0) {
			// Some time left: A full buffer duration?
			if (onsetDelta >= ((double) framesPerBuffer / (double) dev->streaminfo->sampleRate)) {
				// At least one buffer away. Fill our buffer with zeros, aka silence:
				memset(outputBuffer, 0, framesPerBuffer * channels * sizeof(float));
				
				// Ready. Tell engine to continue stream processing, i.e., call us again...
				return(paContinue);
			}
			else {
				// A bit time left, but less than a full buffer. Need to pad the head of
				// this buffer with zeros, aka silence, then fill the rest with real data:
				silenceframes = (unsigned long) (onsetDelta * ((double) dev->streaminfo->sampleRate));

				// Fill in some silence:
				memset(outputBuffer, 0, silenceframes * channels * sizeof(float));
				out+= (silenceframes * channels);

				// Decrement remaining real audio data count:
				framesPerBuffer-=silenceframes;

				// dev->StartTime now exactly corresponds to onset of first non-silence sample.
				// At least if everything works properly.

				// Mark us as running:
				dev->state = 2;
			}
		}
		else {
			// Ooops! We are late! Store real estimated onset in the startTime field and
			// then hurry up!
			dev->startTime = firstsampleonset;

			// Mark us as running:
			dev->state = 2;
		}
	}
    
	// Copy requested number of samples for each channel into the output buffer: Take the case of
	// "loop forever" and "loop repeatCount" times into account:
	for (i=0; (i < framesPerBuffer * channels) && ((repeatCount == -1) || (playposition < (repeatCount * sbsize))); i++) {
		*out++ = dev->buffer[playposition % sbsize];
		playposition++;
	}
	
	// Store updated playposition in device structure:
	dev->playposition = playposition;

	// End of playback reached due to maximum repeatCount reached?
	if (i < framesPerBuffer * channels) {
		// Premature stop of buffer filling because repeatCount exceeded.
		// We need to zero-fill the remainder of the buffer and tell the engine
		// to finish playback:
		while(i < framesPerBuffer * channels) {
			*out++ = 0.0;
			i++;
		}
		
		// Signal that the filled buffers up to this point should be played,
		// but after that the engine should stop the stream:
		return(paComplete);
	}

	// Tell engine to continue stream processing, i.e., call us again...
    return(paContinue);
}

void PsychPACloseStream(int id)
{
	PaStream* stream = audiodevices[id].stream;
	
	if (stream) {
		// Stop, shutdown and release audio stream:
		Pa_StopStream(stream);
		Pa_CloseStream(stream);
		audiodevices[id].stream = NULL;
		
		// Free associated sound buffer:
		if(audiodevices[id].buffer) {
			free(audiodevices[id].buffer);
			audiodevices[id].buffer = NULL;
			audiodevices[id].buffersize = 0;
		}				

		audiodevicecount--;
	}
	
	return;
}

void InitializeSynopsis()
{
	int i=0;
	const char **synopsis = synopsisSYNOPSIS;  //abbreviate the long name
	
	synopsis[i++] = "PsychPortAudio - A sound driver built around the PortAudio sound library:\n";
	synopsis[i++] = "\nGeneral information:\n";
	synopsis[i++] = "version = PsychPortAudio('Version');";
	synopsis[i++] = "oldlevel = PsychPortAudio('Verbosity' [,level]);";
	synopsis[i++] = "devices = PsychPortAudio('GetDevices' [,devicetype]);";
	synopsis[i++] = "status = PsychPortAudio('GetStatus' pahandle);";
	synopsis[i++] = "\n\nDevice setup and shutdown:\n";
	synopsis[i++] = "pahandle = PsychPortAudio('Open' [, deviceid][, mode][, reqlatencyclass][, freq][, channels][, buffersize][, suggestedLatency]);";
	synopsis[i++] = "PsychPortAudio('Close' [, pahandle]);";
	synopsis[i++] = "oldbias = PsychPortAudio('LatencyBias', pahandle [,biasSecs]);";
	synopsis[i++] = "PsychPortAudio('FillBuffer', pahandle, bufferdata);";
	synopsis[i++] = "startTime = PsychPortAudio('Start', pahandle [, repetitions=1] [, when=0] [, waitForStart=0] );";
	synopsis[i++] = "startTime = PsychPortAudio('Stop', pahandle);";

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
	
	return(PsychError_none);
}

void PsychPortAudioInitialize(void)
{
	PaError err;
	int i;
	
	// PortAudio already initialized?
	if (!pa_initialized) {
		if ((err=Pa_Initialize())!=paNoError) {
			printf("PTB-ERROR: Portaudio initialization failed with following port audio error: %s \n", Pa_GetErrorText(err));
			PsychErrorExitMsg(PsychError_system, "Failed to initialize PortAudio subsystem.");
		}
		else {
			if(verbosity>2) printf("PTB-INFO: Using PortAudio engine version: %s\n", Pa_GetVersionText());
		}
		
		for(i=0; i<MAX_PSYCH_AUDIO_DEVS; i++) {
			audiodevices[i].stream = NULL;
		}
		
		audiodevicecount=0;
		
		pa_initialized = TRUE;
	}
}

/* PsychPortAudio('Open') - Open and initialize an audio device via PortAudio.
 */
PsychError PSYCHPORTAUDIOOpen(void) 
{
 	static char useString[] = "pahandle = PsychPortAudio('Open' [, deviceid][, mode][, reqlatencyclass][, freq][, channels][, buffersize][, suggestedLatency]);";
	static char synopsisString[] = 
		"Open a PortAudio audio device and initialize it. Returns a 'pahandle' device handle for the device. "
		"All parameters are optional and have reasonable defaults. 'deviceid' Index to select amongst multiple "
		"logical audio devices supported by PortAudio. Defaults to whatever the systems default sound device is. "
		"Different device id's may select the same physical device, but controlled by a different low-level sound "
		"system. E.g., Windows has about five different sound subsystems. 'mode' Mode of operation. Defaults to "
		"kPortAudioPlayback' i.e., sound playback only. Can be set to 'kPortAudioCapture' for audio "
		"capture, or 'kPortAudioFullDuplex' for simultaneous capture and playback of sound.\n"
		"'reqlatencyclass' Allows to select how aggressive PsychPortAudio should be about minimizing sound latency "
		"and getting good deterministic timing, i.e. how to trade off latency vs. system load and playing nicely "
		"with other sound applications on the system. Level 0 means: Don't care about latency, this mode works always "
		"and with all settings, plays nicely with other sound applications. Level 1 (the default) means: Try to get "
		"the lowest latency that is possible under the constraint of reliable playback, freedom of choice for all parameters and "
		"interoperability with other applications. Level 2 means: Take full control over the audio device, even if this "
		"causes other sound applications to fail or shutdown. Level 3 means: As level 2, but request the most aggressive "
		"settings for the given device. Level 4: Same as 3, but fail if device can't meet the strictest requirements. "
		"'freq' Requested playback/capture rate in samples per second (Hz). Defaults to a value that depends on the "
		"requested latency mode. 'channels' Number of audio channels to use, defaults to 2 for stereo. 'buffersize' "
		"requested size and number of internal audio buffers, smaller numbers mean lower latency but higher system load "
		"and some risk of overloading, which would cause audio dropouts. 'suggestedLatency' optional requested latency in "
		"seconds. PortAudio selects internal operating parameters depending on sampleRate, suggestedLatency and buffersize "
		"as well as device internal properties to optimize for low latency output. Best left alone, only here as manual "
		"override in case all the auto-tuning cleverness fails. ";
		
	static char seeAlsoString[] = "Close GetDeviceSettings ";	 
  	
	int freq, buffersize, channels, latencyclass, mode, deviceid, i;
	double suggestedLatency, lowlatency;
	PaDeviceIndex paDevice;
	PaHostApiIndex paHostAPI;
	PaStreamParameters outputParameters;
	PaStreamParameters inputParameters;
	PaDeviceInfo* inputDevInfo, *outputDevInfo;
	PaStreamFlags sflags;
	PaError err;
	PaStream *stream = NULL;
	
	#if PSYCH_SYSTEM == PSYCH_OSX
		paMacCoreStreamInfo hostapisettings;
	#endif
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(7));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	if (audiodevicecount >= MAX_PSYCH_AUDIO_DEVS) PsychErrorExitMsg(PsychError_user, "Maximum number of simultaneously open audio devices reached.");
	 
	freq = 0;
	buffersize = 0;
	channels = 2;
	latencyclass = 1;
	mode = kPortAudioPlayBack;
	deviceid = -1;
	
	// Make sure PortAudio is online:
	PsychPortAudioInitialize();
	
	// Request optional deviceid:
	PsychCopyInIntegerArg(1, kPsychArgOptional, &deviceid);
	if (deviceid < -1) PsychErrorExitMsg(PsychError_user, "Invalid deviceid provided. Valid values are -1 to maximum number of devices.");

	// Request optional mode of operation:
	PsychCopyInIntegerArg(2, kPsychArgOptional, &mode);
	if (mode < 1 || mode > 3) PsychErrorExitMsg(PsychError_user, "Invalid mode provided. Valid values are 1 to 3.");
	
	if (mode!=1) PsychErrorExitMsg(PsychError_unimplemented, "Sorry, recording and full-duplex mode not yet implemented, only sound playback.");
	
	// Request optional latency class:
	PsychCopyInIntegerArg(3, kPsychArgOptional, &latencyclass);
	if (latencyclass < 0 || latencyclass > 4) PsychErrorExitMsg(PsychError_user, "Invalid reqlatencyclass provided. Valid values are 0 to 4.");
	
	// Request optional frequency:
	PsychCopyInIntegerArg(4, kPsychArgOptional, &freq);
	if (freq < 0 || freq > 200000) PsychErrorExitMsg(PsychError_user, "Invalid frequency provided. Valid values are 0 to 200000 Hz.");
	
	// Request optional number of channels:
	PsychCopyInIntegerArg(5, kPsychArgOptional, &channels);
	if (channels < 1 || channels > 256) PsychErrorExitMsg(PsychError_user, "Invalid number of channels provided. Valid values are 1 to device maximum.");

	// Request optional buffersize:
	PsychCopyInIntegerArg(6, kPsychArgOptional, &buffersize);
	if (buffersize < 0 || buffersize > 4096) PsychErrorExitMsg(PsychError_user, "Invalid buffersize provided. Valid values are 0 to 4096 samples.");

	// Request optional suggestedLatency:
	suggestedLatency = -1.0;
	PsychCopyInDoubleArg(7, kPsychArgOptional, &suggestedLatency);
	if (suggestedLatency!=-1 && (suggestedLatency < 0.0 || suggestedLatency > 1.0)) PsychErrorExitMsg(PsychError_user, "Invalid suggestedLatency provided. Valid values are 0.0 to 1.0 seconds.");

	// FIXME: This needs to handle up to two devices for duplex case, not only one!!!
	if (deviceid == -1) {
		// Default devices requested:
		if (latencyclass == 0) {
			// High latency mode: Simply pick system default devices:
			outputParameters.device = Pa_GetDefaultOutputDevice(); /* Default output device. */
			inputParameters.device  = Pa_GetDefaultInputDevice(); /* Default output device. */
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
	
	// Set channel count:
	outputParameters.channelCount = channels;
	inputParameters.channelCount = channels;
	
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
			buffersize = 64; // Lowest setting that is safe on a fast MacBook-Pro.
		}
	}
	
	// Now we have auto-selected buffersize or user provided override...
	
	// Setup samplerate:
	if (freq == 0) {
		// No specific frequency requested:
		if (latencyclass < 3) {
			// At levels < 3, we select the device specific default.
			freq = outputDevInfo->defaultSampleRate;
		}
		else {
			freq = 96000; // Go really high...
		}
	}
	
	// Now we have auto-selected frequency or user provided override...

	// Set requested latency: In class 0 we choose device recommendation for dropout-free operation, in
	// all higher (lowlat) classes we request zero latency. PortAudio will
	// clamp this request to something safe internally.
	switch (Pa_GetHostApiInfo(Pa_GetDeviceInfo(outputParameters.device)->hostApi)->type) {
		case paCoreAudio:	// CoreAudio driver will automatically clamp to safe minimum. Around 0.7 msecs.
		case paALSA:		// dto. for ALSA.
		case paWDMKS:		// dto. for Windows kernel streaming.
			lowlatency = 0.0;
		break;
		
		case paMME:			// No such a thing as low latency, but easy to kill the machine with too low settings!
			lowlatency = 0.1; // Play safe, request 100 msecs. Otherwise terrible things may happen!
		break;
		
		case paDirectSound:	// DirectSound defaults to 120 msecs, which is way too much! It doesn't accept 0.0 msecs.
			lowlatency = 0.01;	// Choose some half-way safe tradeoff: 10 msecs. Can't go below that anyway.
		break;
		
		default:			// Not the safest assumption for non-verified Api's, but we'll see...
			lowlatency = 0.0;
	}
	
	if (suggestedLatency == -1.0) {
		// None provided: Choose default based on latency mode:
		outputParameters.suggestedLatency = (latencyclass == 0) ? outputDevInfo->defaultHighOutputLatency : lowlatency;
		inputParameters.suggestedLatency  = (latencyclass == 0) ? inputDevInfo->defaultHighInputLatency : lowlatency;
	}
	else {
		// Override provided: Use it.
		outputParameters.suggestedLatency = suggestedLatency;
		inputParameters.suggestedLatency  = suggestedLatency;
	}
	
	// We default to generic system:
	outputParameters.hostApiSpecificStreamInfo = NULL;
	inputParameters.hostApiSpecificStreamInfo  = NULL;

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
	
	// Our stream shall be primed initially with our callbacks data, not just with zeros.
	// In high latency-mode 0, we request sample clipping and dithering, so sound is more
	// high quality on Windows. In low-latency mode, we safe the computation time for that.
	sflags = paPrimeOutputBuffersUsingStreamCallback;
	sflags = (latencyclass > 0) ? sflags : (sflags | paClipOff | paDitherOff);

	// Try to create & open stream:
	err = Pa_OpenStream(
            &stream,													/* Return stream pointer here on success. */
            ((mode & kPortAudioCapture) ?  &inputParameters : NULL),	/* Requested input settings, or NULL in pure playback case. */
			((mode & kPortAudioPlayBack) ? &outputParameters : NULL),	/* Requested input settings, or NULL in pure playback case. */
            freq,														/* Requested sampling rate. */
            buffersize,													/* Requested buffer size. */
            sflags,														/* Define special stream property flags. */
            paCallback,													/* Our processing callback. */
            &audiodevices[audiodevicecount]);							/* Our own device info structure */
            
	if(err!=paNoError || stream == NULL) {
			printf("PTB-ERROR: Failed to open audio device %i. PortAudio reports this error: %s \n", audiodevicecount, Pa_GetErrorText(err));
			PsychErrorExitMsg(PsychError_system, "Failed to open PortAudio audio device.");
	}
	
	// Setup our final device structure:
	audiodevices[audiodevicecount].stream = stream;
	audiodevices[audiodevicecount].streaminfo = Pa_GetStreamInfo(stream);
	audiodevices[audiodevicecount].hostAPI = Pa_GetHostApiInfo(Pa_GetDeviceInfo(outputParameters.device)->hostApi)->type;
	audiodevices[audiodevicecount].startTime = 0.0;
	audiodevices[audiodevicecount].state = 0;
	audiodevices[audiodevicecount].repeatCount = 1;
	audiodevices[audiodevicecount].buffer = NULL;
	audiodevices[audiodevicecount].buffersize = 0;
	audiodevices[audiodevicecount].channels = channels;
	audiodevices[audiodevicecount].latencyBias = 0.0;
	
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
		printf("PTB-INFO: Audio subsystem is %s, Audio device name is ", Pa_GetHostApiInfo(Pa_GetDeviceInfo(outputParameters.device)->hostApi)->name);
		printf("%s\n", Pa_GetDeviceInfo(outputParameters.device)->name);
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

/* PsychPortAudio('FillBuffer') - Fill audio buffer of a device with data.
 */
PsychError PSYCHPORTAUDIOFillAudioBuffer(void) 
{
 	static char useString[] = "PsychPortAudio('FillBuffer', pahandle, bufferdata);";
	static char synopsisString[] = 
		"Fill audio data playback buffer of a PortAudio audio device. 'pahandle' is the handle of the device "
		"whose buffer is to be filled. 'bufferdata' is a Matlab double matrix with audio data in double format. Each "
		"row of the matrix specifies one sound channel, each column one sample for each channel. Only floating point "
		"values in double precision are supported. Samples need to be in range -1.0 to +1.0, 0.0 for silence. This is "
		"intentionally a very restricted interface. For lowest latency and best timing we want you to provide audio "
		"data exactly at the optimal format and sample rate, so the driver can safe computation time and latency for "
		"expensive sample rate conversion, sample format conversion, and bounds checking/clipping. ";
	static char seeAlsoString[] = "Open GetDeviceSettings ";	 
  	
	int inchannels, insamples, p, buffersize;
	double*	indata = NULL;
	float*  outdata = NULL;
	int pahandle   = -1;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(2));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(2)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(0));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");

	PsychAllocInDoubleMatArg(2, kPsychArgRequired, &inchannels, &insamples, &p, &indata);
	if (inchannels != audiodevices[pahandle].channels) {
		printf("PTB-ERROR: Audio device %i has %i output channels, but provided matrix has non-matching number of %i rows.\n", pahandle, audiodevices[pahandle].channels, inchannels);
		PsychErrorExitMsg(PsychError_user, "Number of rows of audio data matrix doesn't match number of output channels of selected audio device.\n");
	}
	
	if (insamples < 1) PsychErrorExitMsg(PsychError_user, "You must provide at least 1 sample in you audio buffer!");
	if (p!=1) PsychErrorExitMsg(PsychError_user, "Audio data matrix must be a 2D matrix, but this one is not a 2D matrix!");
	
	// Wait for playback on this stream to finish, before refilling it:
	while (audiodevices[pahandle].state > 0) {
		// Sleep a millisecond:
		PsychWaitIntervalSeconds(0.001);
	}

	// Ok, everything sane, fill the buffer:
	buffersize = sizeof(float) * inchannels * insamples;
	if (audiodevices[pahandle].buffer && (audiodevices[pahandle].buffersize != buffersize)) {
		free(audiodevices[pahandle].buffer);
		audiodevices[pahandle].buffer = NULL;
		audiodevices[pahandle].buffersize = 0;
	}
	
	if (audiodevices[pahandle].buffer == NULL) {
		audiodevices[pahandle].buffersize = buffersize;
		audiodevices[pahandle].buffer = (float*) malloc(buffersize);
		if (audiodevices[pahandle].buffer==NULL) PsychErrorExitMsg(PsychError_outofMemory, "Out of system memory when trying to allocate audio buffer.");
	}
	
	audiodevices[pahandle].playposition = 0;
	
	// Copy the data, convert it from double to float:
	outdata = audiodevices[pahandle].buffer;
	while(buffersize) {
		*(outdata++) = (float) *(indata++);
		buffersize-=sizeof(float);
	}
	
	// Buffer ready.
	return(PsychError_none);
}

/* PsychPortAudio('Start') - Start an audio device via PortAudio.
 */
PsychError PSYCHPORTAUDIOStartAudioDevice(void) 
{
 	static char useString[] = "startTime = PsychPortAudio('Start', pahandle [, repetitions=1] [, when=0] [, waitForStart=0] );";
	static char synopsisString[] = 
		"Start a PortAudio audio device. The 'pahandle' is the handle of the device to start. Starting a "
		"device means: Start playback of output devices, start recording on capture device, do both on "
		"full duplex devices. 'waitForStart' if set to 1 will wait until device has really started, default "
		"is to continue immediately, ie. only schedule start of device. 'when' Requested time, when device "
		"should start. Defaults to zero, i.e. start immediately. If set to a non-zero system time, PTB will "
		"do its best to start the device at the requested time, but the accuracy of start depends on the "
		"operating system, audio hardware and system load. If 'waitForStart' is set to non-zero value, ie "
		"if PTB should wait for sound onset, then the optional return argument 'startTime' will contain an "
		"estimate of when the first audio sample hit the speakers, i.e., the real start time. ";
	static char seeAlsoString[] = "Open GetDeviceSettings ";	 
  	
	PaError err;
	int pahandle= -1;
	int waitForStart = 0;
	int repetitions = 1;
	double when = 0.0;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(4));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");
	if (audiodevices[pahandle].buffer == NULL) PsychErrorExitMsg(PsychError_user, "Sound buffer doesn't contain any sound to play?!?");
	if (audiodevices[pahandle].state > 0) PsychErrorExitMsg(PsychError_user, "Device already started.");

	PsychCopyInIntegerArg(2, kPsychArgOptional, &repetitions);
	if (repetitions < 0) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'repetitions'. Valid values are zero or greater.");

	// Set number of requested repetitions: 0 means loop forever, default is 1 time.
	audiodevices[pahandle].repeatCount = (repetitions == 0) ? -1 : repetitions;

	PsychCopyInDoubleArg(3, kPsychArgOptional, &when);
	if (when < 0) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'when'. Valid values are zero or greater.");
	
	// Setup target start time:
	audiodevices[pahandle].startTime = when;

	PsychCopyInIntegerArg(4, kPsychArgOptional, &waitForStart);
	if (waitForStart < 0) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'waitForStart'. Valid values are zero or greater.");
	
	// Reset statistics values:
	audiodevices[pahandle].batchsize = 0;	
	audiodevices[pahandle].xruns = 0;	
	audiodevices[pahandle].paCalls = 0;
	audiodevices[pahandle].noTime = 0;
	
	// Mark state as hot-started:
	audiodevices[pahandle].state = 1;

	// Try to start stream:
	if ((err=Pa_StartStream(audiodevices[pahandle].stream))!=paNoError) {
		printf("PTB-ERROR: Failed to start audio device %i. PortAudio reports this error: %s \n", pahandle, Pa_GetErrorText(err));
		PsychErrorExitMsg(PsychError_system, "Failed to start PortAudio audio device.");
	}
		
	if (waitForStart>0) {
		// Wait for real start of device:
		while(audiodevices[pahandle].state != 2) {
			PsychWaitIntervalSeconds(0.001);
		}
		
		// Ok, relevant audio buffer with real sound onset submit to engine.
		// We now have an estimate of real sound onset in startTime, wait until
		// then:
		PsychWaitUntilSeconds(audiodevices[pahandle].startTime);
		
		// Engine should run now. Return real onset time:
		PsychCopyOutDoubleArg(1, kPsychArgOptional, audiodevices[pahandle].startTime);
	}
	else {
		// Return empty zero timestamp to signal that this info is not available:
		PsychCopyOutDoubleArg(1, kPsychArgOptional, 0.0);
	}

	return(PsychError_none);
}

/* PsychPortAudio('Stop') - Stop an audio device via PortAudio.
 */
PsychError PSYCHPORTAUDIOStopAudioDevice(void) 
{
 	static char useString[] = "[startTime endPositionSecs xruns]= PsychPortAudio('Stop', pahandle);";
	static char synopsisString[] = 
		"Stop a PortAudio audio device. The 'pahandle' is the handle of the device to stop. "
		"The optional return argument 'startTime' returns an estimate of when the stopped "
		"stream actually started its playback. Its the same timestamp returned by the start "
		"command when executed in waiting mode. 'endPositionSecs' is the final playback "
		"position in seconds. 'xruns' is the number of buffer over- or underruns. This should "
		"be zero if the playback was glitch-free, however a zero value doesn't imply glitch "
		"free playback, as the algorithm can miss some types of glitches. ";		
	static char seeAlsoString[] = "Open GetDeviceSettings ";	 
	
	PaError err;
	int pahandle= -1;

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(1));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(3));	 // The maximum number of outputs

	// Make sure PortAudio is online:
	PsychPortAudioInitialize();

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");

	// Try to stop stream:
	if ((err=Pa_StopStream(audiodevices[pahandle].stream))!=paNoError) {
		printf("PTB-ERROR: Failed to stop audio device %i. PortAudio reports this error: %s \n", pahandle, Pa_GetErrorText(err));
		PsychErrorExitMsg(PsychError_system, "Failed to stop PortAudio audio device.");
	}
	
	// Wait for real stop:
	while (Pa_IsStreamActive(audiodevices[pahandle].stream)) PsychWaitIntervalSeconds(0.001);
	
	// Copy out our estimate of when playback really started for the just stopped stream:
	PsychCopyOutDoubleArg(1, kPsychArgOptional, audiodevices[pahandle].startTime);

	// Copy out final playback position (secs) since start:
	PsychCopyOutDoubleArg(2, kPsychArgOptional, ((double)(audiodevices[pahandle].playposition / audiodevices[pahandle].channels)) / (double) audiodevices[pahandle].streaminfo->sampleRate);

	// Copy out number of buffer over-/underflows since start:
	PsychCopyOutDoubleArg(3, kPsychArgOptional, audiodevices[pahandle].xruns);
	
	// Mark state as stopped:
	audiodevices[pahandle].state = 0;

	// Reset play position:
	audiodevices[pahandle].playposition = 0;
	

	return(PsychError_none);
}

/* PsychPortAudio('GetStatus') - Return current status of stream.
 */
PsychError PSYCHPORTAUDIOGetStatus(void) 
{
 	static char useString[] = "status = PsychPortAudio('GetStatus', pahandle);";
	static char synopsisString[] = 
		"Returns 'status', a struct with status information about the current state of device 'pahandle'. "
		"Playing: Can be 1 if playback is active, or 0 if playback is stopped or not yet started. "
		"StartTime: Is the real start time of the audio stream after start of playback. PositionSecs is "
		"an estimate of the current stream playback position in seconds, its not totally accurate, because "
		"it measures how much sound has been submitted to the sound system, not how much sound has left the "
		"speakers, i.e., it doesn't take driver and hardware latency into account. XRuns: Number of dropouts "
		"due to buffer overrun or underrun conditions. TotalCalls, TimeFailed and BufferSize are only for "
		"debugging of PsychPortAudio itself. CPULoad: How much load does the playback engine impose on the "
		"CPU? Values can range from 0.0 = 0% to 1.0 for 100%. Values close to 1.0 indicate that your system "
		"can't handle the load and timing glitches or sound glitches are likely. In such a case, try to reduce "
		"the load on your system. PredictedLatency: Is the latency of your driver+hardware combo. It tells you, "
		"how far ahead of time a sound device must be started ahead of the requested onset time via "
		"PsychPortAudio('Start'...) to make sure it actually starts playing in time. High quality systems like "
		"Linux or MacOS/X may allow values as low as 5 msecs or less on standard hardware. Other operating "
		"systems may require dozens or hundreds of milliseconds of headstart. LatencyBias is an additional bias "
		"setting you can impose via PsychPortAudio('LatencyBias', pahandle, bias); in case our drivers estimate "
		"is a bit off. Allows fine-tuning.";
		
	static char seeAlsoString[] = "Open GetDeviceSettings ";	 
	PsychGenericScriptType 	*status;
	const char *FieldNames[]={	"Playing", "StartTime", "PositionSecs", "XRuns", "TotalCalls", "TimeFailed", "BufferSize", "CPULoad", "PredictedLatency",
								"LatencyBias"};
	int pahandle = -1;
	int count = 0;
	int i;
	PaDeviceInfo* padev = NULL;
	PaHostApiInfo* hainfo = NULL;
	
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

	PsychAllocOutStructArray(1, kPsychArgOptional, 1, 10, FieldNames, &status);
	
	PsychSetStructArrayDoubleElement("Playing", 0, (audiodevices[pahandle].state == 2) ? 1 : 0, status);
	PsychSetStructArrayDoubleElement("StartTime", 0, audiodevices[pahandle].startTime, status);
	PsychSetStructArrayDoubleElement("PositionSecs", 0, ((double)(audiodevices[pahandle].playposition / audiodevices[pahandle].channels)) / (double) audiodevices[pahandle].streaminfo->sampleRate, status);
	PsychSetStructArrayDoubleElement("XRuns", 0, audiodevices[pahandle].xruns, status);
	PsychSetStructArrayDoubleElement("TotalCalls", 0, audiodevices[pahandle].paCalls, status);
	PsychSetStructArrayDoubleElement("TimeFailed", 0, audiodevices[pahandle].noTime, status);
	PsychSetStructArrayDoubleElement("BufferSize", 0, audiodevices[pahandle].batchsize, status);
	PsychSetStructArrayDoubleElement("CPULoad", 0, Pa_GetStreamCpuLoad(audiodevices[pahandle].stream), status);
	PsychSetStructArrayDoubleElement("PredictedLatency", 0, audiodevices[pahandle].predictedLatency, status);
	PsychSetStructArrayDoubleElement("LatencyBias", 0, audiodevices[pahandle].latencyBias, status);
	return(PsychError_none);
}

/* Logger callback function to output PortAudio debug messages at 'verbosity' > 5. */
void PALogger(const char* msg)
{
	printf("PTB-DEBUG: PortAudio says: %s", msg);
	return;
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
	
	if (verbosity > 5) PaUtil_SetDebugPrintFunction(PALogger);

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
