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

typedef struct PsychPADevice {
	PaStream *stream;			// Pointer to associated portaudio stream.
	PaStreamInfo* streaminfo;   // Pointer to stream info structure, provided by PortAudio.
	double	 startTime;			// Requested playback start time in system time (secs).
	int		 state;				// Current state of the stream: 0=Stopped, 1=Hot Standby, 2=Playing.
	int		 repeatCount;		// Number of repetitions: -1 = Loop forever, 1 = Once, n = n repetitions.
	float*	 buffer;
	int		 buffersize;
	unsigned int playposition;
	unsigned int channels;
} PsychPADevice;

PsychPADevice audiodevices[MAX_PSYCH_AUDIO_DEVS];
unsigned int  audiodevicecount;
unsigned int  verbosity = 4;

boolean pa_initialized = FALSE;

/* Very simple synthesis routine to generate two sine waves. */
static int paOutCallback( const void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void *userData )
{
    PsychPADevice* dev = (PsychPADevice*) userData;
    float *out = (float*) outputBuffer;
    float *in = (float*) inputBuffer;
    unsigned long i;
	unsigned long channels;
	unsigned int playposition;
	
	// Sound output buffer attached to stream?
    if (dev == NULL || dev->buffer == NULL) return(0);
	
	dev->state = 2;
	
	// Query number of output channels:
	channels = (unsigned long) dev->channels;
	playposition = dev->playposition;
	
	// Compute size of soundbuffer in samples:
    unsigned int sbsize = dev->buffersize / sizeof(float);
    
	// Copy requested number of samples:
    for (i=0; i < framesPerBuffer * channels; i++) {
        *out++ = dev->buffer[playposition % sbsize];
        playposition++;
    }
    
	dev->playposition = playposition;
	
    return(0);
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
	
	synopsis[i++] = "PsychPortAudio - Subfunctions overview:";
	synopsis[i++] = "\n\nGeneral information:\n";
	synopsis[i++] = "version = PsychPortAudio('Version');";
	synopsis[i++] = "\n\nDevice setup and shutdown:\n";
	synopsis[i++] = "pahandle = PsychPortAudio('OpenAudioDevice' [, deviceid][, mode][, reqlatencyclass][, freq][, channels][, buffersize]);";
	synopsis[i++] = "PsychPortAudio('Close' [, pahandle]);";
	synopsis[i++] = "PsychPortAudio('FillAudioBuffer', pahandle, bufferdata);";
	synopsis[i++] = "PsychPortAudio('StartAudioDevice', pahandle [, waitForStart=0] [, when=0]);";
	synopsis[i++] = "PsychPortAudio('StopAudioDevice', pahandle);";

	synopsis[i++] = NULL;  //this tells PsychDisplayScreenSynopsis where to stop
	if (i > MAX_SYNOPSIS_STRINGS) {
		PrintfExit("%s: increase dimension of synopsis[] from %ld to at least %ld and recompile.",__FILE__,(long)MAX_SYNOPSIS_STRINGS,(long)i);
	}
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

		// Shutdown PortAudio itself:
		err = Pa_Terminate();
		if (err) {
			printf("PTB-FATAL-ERROR: PsychPortAudio: Shutdown of port audio subsystem failed. Depending on the quality\n");
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

/* PsychPortAudio('OpenAudioDevice') - Open and initialize an audio device via PortAudio.
 */
PsychError PSYCHPORTAUDIOOpen(void) 
{
 	static char useString[] = "pahandle = PsychPortAudio('OpenAudioDevice' [, deviceid][, mode][, reqlatencyclass][, freq][, channels][, buffersize]);";
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
		"and some risk of overloading, which would cause audio dropouts. ";
	static char seeAlsoString[] = "Close GetDeviceSettings ";	 
  	
	int freq, buffersize, channels, latencyclass, mode, deviceid, i;
	PaDeviceIndex paDevice;
	PaStreamParameters outputParameters;
	PaStreamParameters inputParameters;
	PaDeviceInfo* inputDevInfo, *outputDevInfo;
	PaStream *stream = NULL;
	
	#if PSYCH_SYSTEM == PSYCH_OSX
		paMacCoreStreamInfo hostapisettings;
	#endif
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(6));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	if (audiodevicecount >= MAX_PSYCH_AUDIO_DEVS) PsychErrorExitMsg(PsychError_user, "Maximum number of simultaneously open audio devices reached.");
	 
	freq = 0;
	buffersize = 0;
	channels = 2;
	latencyclass = 1;
	mode = kPortAudioPlayBack;
	deviceid = -1;
	PaError err;
	
	// PortAudio already initialized?
	if (!pa_initialized) {
		if ((err=Pa_Initialize())!=paNoError) {
			printf("PTB-ERROR: Portaudio initialization failed with following port audio error: %s \n", Pa_GetErrorText(err));
			PsychErrorExitMsg(PsychError_system, "Failed to initialize PortAudio subsystem.");
		}
		
		for(i=0; i<MAX_PSYCH_AUDIO_DEVS; i++) {
			audiodevices[i].stream = NULL;
		}
		
		pa_initialized = TRUE;
	}
	
	// Request optional deviceid:
	PsychCopyInIntegerArg(1, kPsychArgOptional, &deviceid);
	if (deviceid < -1) PsychErrorExitMsg(PsychError_invalidIntegerArg, "Invalid deviceid provided. Valid values are -1 to maximum number of devices.");

	// Request optional mode of operation:
	PsychCopyInIntegerArg(2, kPsychArgOptional, &mode);
	if (mode < 1 || mode > 3) PsychErrorExitMsg(PsychError_invalidIntegerArg, "Invalid mode provided. Valid values are 1 to 3.");
	
	// Request optional latency class:
	PsychCopyInIntegerArg(3, kPsychArgOptional, &latencyclass);
	if (latencyclass < 0 || latencyclass > 4) PsychErrorExitMsg(PsychError_invalidIntegerArg, "Invalid reqlatencyclass provided. Valid values are 0 to 4.");
	
	// Request optional frequency:
	PsychCopyInIntegerArg(4, kPsychArgOptional, &freq);
	if (freq < 0 || freq > 200000) PsychErrorExitMsg(PsychError_invalidIntegerArg, "Invalid frequency provided. Valid values are 0 to 200000 Hz.");
	
	// Request optional number of channels:
	PsychCopyInIntegerArg(5, kPsychArgOptional, &channels);
	if (channels < 1 || channels > 256) PsychErrorExitMsg(PsychError_invalidIntegerArg, "Invalid number of channels provided. Valid values are 1 to device maximum.");

	// Request optional buffersize:
	PsychCopyInIntegerArg(6, kPsychArgOptional, &buffersize);
	if (buffersize < 0 || buffersize > 4096) PsychErrorExitMsg(PsychError_invalidIntegerArg, "Invalid buffersize provided. Valid values are 0 to 4096 samples.");

	// FIXME: This needs to handle up to two devices for duplex case, not only one!!!
	if (deviceid == -1) {
		// Default device requested:
		outputParameters.device = Pa_GetDefaultOutputDevice(); /* Default output device. */
		inputParameters.device  = Pa_GetDefaultInputDevice(); /* Default output device. */
	}
	else {
		// Specific device requested: In valid range?
		if (deviceid >= Pa_GetDeviceCount()) {
			PsychErrorExitMsg(PsychError_invalidIntegerArg, "Invalid deviceid provided. Higher than the number of devices - 1.");
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
			buffersize = 32; // Dangerously low, but no risk, no fun...
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
			freq = 192000; // Go really high...
		}
	}
	
	// Now we have auto-selected frequency or user provided override...

	// Set requested latency: In class 0 we choose device recommendation for dropout-free operation, in
	// all higher (lowlat) classes we request zero latency. PortAudio will
	// clamp this request to something safe internally.
	outputParameters.suggestedLatency = (latencyclass == 0) ? outputDevInfo->defaultHighOutputLatency : 0.0;
	inputParameters.suggestedLatency  = (latencyclass == 0) ? inputDevInfo->defaultHighInputLatency : 0.0;
	
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

	// Try to create & open stream:
	err = Pa_OpenStream(
            &stream,													/* Return stream pointer here on success. */
            ((mode & kPortAudioCapture) ?  &inputParameters : NULL),	/* Requested input settings, or NULL in pure playback case. */
			((mode & kPortAudioPlayBack) ? &outputParameters : NULL),	/* Requested input settings, or NULL in pure playback case. */
            freq,														/* Requested sampling rate. */
            buffersize,													/* Requested buffer size. */
            paClipOff | paPrimeOutputBuffersUsingStreamCallback,		/* Don't clip out of range samples, prime outbuffers from our data. */
            paOutCallback,												/* Our processing callback. */
            &audiodevices[audiodevicecount]);							/* Our own device info structure */
            
	if(err!=paNoError || stream == NULL) {
			printf("PTB-ERROR: Failed to open audio device %i. PortAudio reports this error: %s \n", audiodevicecount, Pa_GetErrorText(err));
			PsychErrorExitMsg(PsychError_system, "Failed to open PortAudio audio device.");
	}
	
	// Setup our final device structure:
	audiodevices[audiodevicecount].stream = stream;
	audiodevices[audiodevicecount].streaminfo = Pa_GetStreamInfo(stream);
	audiodevices[audiodevicecount].startTime = 0.0;
	audiodevices[audiodevicecount].state = 0;
	audiodevices[audiodevicecount].repeatCount = 1;
	audiodevices[audiodevicecount].buffer = NULL;
	audiodevices[audiodevicecount].buffersize = 0;
	audiodevices[audiodevicecount].channels = channels;
	
	if (verbosity > 3) {
		printf("PTB-INFO: New audio device with handle %i opened as PortAudio stream:\n", audiodevicecount);
		printf("PTB-INFO: Real samplerate %f Hz. Input latency %f msecs, Output latency %f msecs.\n",
				audiodevices[audiodevicecount].streaminfo->sampleRate, audiodevices[audiodevicecount].streaminfo->inputLatency,
				audiodevices[audiodevicecount].streaminfo->outputLatency);
	}
	
	// Return device handle:
	PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) audiodevicecount);
	
	// One more...
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
	static char seeAlsoString[] = "OpenAudioDevice GetDeviceSettings ";	 
  	
	int pahandle= -1;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(1));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(0));	 // The maximum number of outputs

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

/* PsychPortAudio('FillAudioBuffer') - Fill audio buffer of a device with data.
 */
PsychError PSYCHPORTAUDIOFillAudioBuffer(void) 
{
 	static char useString[] = "PsychPortAudio('FillAudioBuffer', pahandle, bufferdata);";
	static char synopsisString[] = 
		"Fill audio data playback buffer of a PortAudio audio device. 'pahandle' is the handle of the device "
		"whose buffer is to be filled. 'bufferdata' is a Matlab matrix with audio data in double format. Each "
		"row of the matrix specifies one sound channel, each column one sample for each channel. ";
	static char seeAlsoString[] = "OpenAudioDevice GetDeviceSettings ";	 
  	
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

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");

	PsychAllocInDoubleMatArg(2, kPsychArgRequired, &inchannels, &insamples, &p, &indata);
	if (inchannels != audiodevices[pahandle].channels) {
		printf("PTB-ERROR: Audio device %i has %i output channels, but provided matrix has non-matching number of %i rows.", pahandle, audiodevices[pahandle].channels, inchannels);
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

/* PsychPortAudio('StartAudioDevice') - Start an audio device via PortAudio.
 */
PsychError PSYCHPORTAUDIOStartAudioDevice(void) 
{
 	static char useString[] = "PsychPortAudio('StartAudioDevice', pahandle [, waitForStart=0] [, when=0]);";
	static char synopsisString[] = 
		"Start a PortAudio audio device. The 'pahandle' is the handle of the device to start. Starting a "
		"device means: Start playback of output devices, start recording on capture device, do both on "
		"full duplex devices. 'waitForStart' if set to 1 will wait until device has really started, default "
		"is to continue immediately, ie. only schedule start of device. 'when' Requested time, when device "
		"should start. Defaults to zero, i.e. start immediately. If set to a non-zero system time, PTB will "
		"do its best to start the device at the requested time, but the accuracy of start depends on the "
		"operating system, audio hardware and system load. ";
	static char seeAlsoString[] = "OpenAudioDevice GetDeviceSettings ";	 
  	
	PaError err;
	int pahandle= -1;
	int waitForStart = 0;
	double when = 0.0;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(3));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(0));	 // The maximum number of outputs

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");
	if (audiodevices[pahandle].buffer == NULL) PsychErrorExitMsg(PsychError_user, "Sound buffer doesn't contain any sound to play?!?");
	if (audiodevices[pahandle].state > 0) PsychErrorExitMsg(PsychError_user, "Device already started.");
	
	PsychCopyInIntegerArg(2, kPsychArgOptional, &waitForStart);
	if (waitForStart < 0) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'waitForStart'. Valid values are zero or greater.");
	
	PsychCopyInDoubleArg(3, kPsychArgOptional, &when);
	if (when < 0) PsychErrorExitMsg(PsychError_user, "Invalid setting for 'when'. Valid values are zero or greater.");
	
	// Setup target start time:
	audiodevices[pahandle].startTime = when;
	
	// Try to start stream:
	if ((err=Pa_StartStream(audiodevices[pahandle].stream))!=paNoError) {
		printf("PTB-ERROR: Failed to start audio device %i. PortAudio reports this error: %s \n", pahandle, Pa_GetErrorText(err));
		PsychErrorExitMsg(PsychError_system, "Failed to start PortAudio audio device.");
	}
	
	// Mark state as hot-started:
	audiodevices[pahandle].state = 1;
	
	if (waitForStart>0) {
		// Wait for real start of device:
		while(audiodevices[pahandle].state != 2) {
			PsychWaitIntervalSeconds(0.001);
		}
	}

	return(PsychError_none);
}

/* PsychPortAudio('StopAudioDevice') - Stop an audio device via PortAudio.
 */
PsychError PSYCHPORTAUDIOStopAudioDevice(void) 
{
 	static char useString[] = "PsychPortAudio('StopAudioDevice', pahandle);";
	static char synopsisString[] = 
		"Stop a PortAudio audio device. The 'pahandle' is the handle of the device to stop. ";		
	static char seeAlsoString[] = "OpenAudioDevice GetDeviceSettings ";	 
	
	PaError err;
	int pahandle= -1;

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(1));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(0));	 // The maximum number of outputs

	PsychCopyInIntegerArg(1, kPsychArgRequired, &pahandle);
	if (pahandle < 0 || pahandle>=MAX_PSYCH_AUDIO_DEVS || audiodevices[pahandle].stream == NULL) PsychErrorExitMsg(PsychError_user, "Invalid audio device handle provided.");
	if (audiodevices[pahandle].state == 0) PsychErrorExitMsg(PsychError_user, "Device already stopped.");
	
	// Try to stop stream:
	if ((err=Pa_StopStream(audiodevices[pahandle].stream))!=paNoError) {
		printf("PTB-ERROR: Failed to stop audio device %i. PortAudio reports this error: %s \n", pahandle, Pa_GetErrorText(err));
		PsychErrorExitMsg(PsychError_system, "Failed to stop PortAudio audio device.");
	}
	
	// Wait for real stop:
	while (Pa_IsStreamActive(audiodevices[pahandle].stream)) PsychWaitIntervalSeconds(0.001);

	// Mark state as stopped:
	audiodevices[pahandle].state = 0;

	// Reset play position:
	audiodevices[pahandle].playposition = 0;

	return(PsychError_none);
}