/*
	PsychToolbox3/Source/Common/PsychSound/PsychSound.c		
 
	PROJECTS: PsychSound only.  
 
	AUTHORS:
 
        mario.kleiner at tuebingen.mpg.de   mk
 
	PLATFORMS:	OS X only for now, but should work on all platforms.  
 
	HISTORY:
 
        10/14/05	mk		Created 
 
        HOWTO BUILD:
 
        This is a bit ugly at the moment:
 
        PsychSound needs to be linked against the libOpenAL.ar static library in StaticOpenALLib.
        -> If linker complains, remove this lib from Target and readd it. Might be necessary to
        remove the whole StaticOpenALLib folder/group and readd it for some weird reason that
        only the makers of XCode understand!?!?
 
        If for some reason libOpenAL.ar needs to be rebuild from scratch, execute the following
        command in a console to rebuild from object files:
 
        ar -r libOpenAL.ar *.o

        If the objects need to be rebuilt as well, unzip the OpenALSourcecode.zip file contained
        in StaticOpenALLib and execute:
 
        make -f StaticMakefileMK
 
        This will build the .o files and libOpenAL.ar
 
        Repeat all steps above as necessary.
 
        This procedure is pretty ugly, but i couldn't find a quick good way to do it properly in
        fu%$*!$$!g XCode.
 
        If Apple ever manages to provide a decent implementation of OpenAL in their
        OpenAL.framework for OS-X ("The most advanced operating system in the world" lol)
        we can remove this crud and just add the OpenAL.framework...
 
        Unfortunately the OpenAL.framework shipped with 10.4.3 still lacks support for the
        Audio capture extensions...
 
	DESCRIPTION: 
 
        Implementation of the PsychSound function, which is supposed to be a
        replacement for the old PTB SND() function.
 
        Design goals:
        - More flexibility, more functionality than old SND function.
        - Fully asynchronous operation - choice between sync/async mode.
        - Better timing, aka lowe latencies in start/stop/processing of sound playback or recording.
        - Bugfree(TM), as opposed to the very buggy Matlab drivers on Windows and MacOS-X.
        - Fully portable due to the use of OpenAL as underlying sound library (Available for OS-X, Windows, Linux, Unixes, ...)
 
	TARGET LOCATION:
 
        PsychSound.mexmac resides in:
        PsychToolbox/PsychBasic/PsychSound.mexmac
 */


#include "PsychSound.h"

#define MAX_SYNOPSIS_STRINGS 500  

//declare variables local to this file.  
static const char *synopsisSYNOPSIS[MAX_SYNOPSIS_STRINGS];

static ALCdevice* (*alcCaptureOpenDevice)(const ALubyte *deviceName,
                                          ALuint freq, ALenum fmt,
                                          ALsizei bufsize);
static ALvoid (*alcCaptureCloseDevice)(ALCdevice *device);
static ALvoid (*alcCaptureStart)(ALCdevice *device);
static ALvoid (*alcCaptureStop)(ALCdevice *device);
static ALvoid (*alcCaptureSamples)(ALCdevice *device, ALvoid *buf,
                                   ALsizei samps);

ALenum _AL_FORMAT_MONO_FLOAT32 = 0;
ALenum _AL_FORMAT_STEREO_FLOAT32 = 0;

#define FMT AL_FORMAT_MONO16;
#define FMTSIZE 8
#define FREQ 44100
#define SAMPS (FREQ * 5)
static ALbyte buf[SAMPS * FMTSIZE];

// Pointer to our recording device, NULL = No rec. device open.
static ALCdevice *in = NULL;
static ALboolean ext;
static ALuint sid;
static ALuint bid;
static ALint state = AL_PLAYING;
static ALenum capture_samples = 0;

// Counter for number of available samples in recording-buffer:
static ALint samples = 0;
static ALint recording_state = 0;
static unsigned int convbuffersize = 0;
static unsigned char* convbuffer = NULL;
static int channels, resolution;
int recording_freq;

void InitializeSynopsis()
{
    int i=0;
    const char **synopsis = synopsisSYNOPSIS;  //abbreviate the long name
    
    synopsis[i++] = "Usage:";
    
    // Subfunctions for management of audio capture / recording devices:
    synopsis[i++] = "\n% Manage and use sound recording devices:";
    synopsis[i++] = "PsychSound('InitRecording');";	
    synopsis[i++] = "PsychSound('StartRecording');";	
    synopsis[i++] = "PsychSound('StopRecording');";	
    synopsis[i++] = "PsychSound('ShutdownRecording');";	
    synopsis[i++] = "PsychSound('GetRecordingPosition');";	
    synopsis[i++] = "PsychSound('GetData');";	
    
    synopsis[i++] = NULL;  //this tells PsychDisplaySoundSynopsis where to stop
    if (i > MAX_SYNOPSIS_STRINGS) {
        PrintfExit("%s: increase dimension of synopsis[] from %ld to at least %ld and recompile.",__FILE__,(long)MAX_SYNOPSIS_STRINGS,(long)i);
    }

    printf("\n\nThis is the PsychSound() Prototype - Version 0.1 ALPHA written 2005 by Mario Kleiner.\n");
    printf("This release is a prototype/draft implementation of the new sound function for PTB-OSX.\n");
    printf("It should be already useful for basic sound recording, but is only mildly tested and\n");
    printf("virtually guaranteed to contain bugs, limitations and loose ends. USE AT YOUR OWN RISK!\n");
    printf("Built upon the OpenAL audio library (see http://www.openal.org for infos) for maximum\n");
    printf("flexibility and portability.\n");
    printf("OpenAL-Info: Version %s - Vendor %s - Renderer %s - Extensions %s.\n", alGetString(AL_VERSION),
           alGetString(AL_VENDOR), alGetString(AL_RENDERER), alGetString(AL_EXTENSIONS));
    
}

PsychError PsychDisplaySoundSynopsis(void)
{
    int i;
    
    for (i = 0; synopsisSYNOPSIS[i] != NULL; i++)
        printf("%s\n",synopsisSYNOPSIS[i]);
    
    return(PsychError_none);
}

PsychError PSYCHSOUNDInitRecording(void)
{
    static char useString[] = "recorderPtr = PsychSound('InitRecording' [,freq=44100] [,expecteddurationsecs=5] [,numchannels=1] [,resolution=2]);"; 
    static char synopsisString[] = "Prepare for continous recording of sound data from default sound device. "
    "recorderPtr=Handle to recording device. This initial call can take significant time. freq=Sampling frequency in Hz. "
    "exptecteddurationsecs=Psychtoolbox will buffer sounddata in an internal buffer for this amount of time (default 5 secs) "
    "for you. You can read out sound via 'GetData' anytime during the recording or after the recording. If you let pass "
    "more than expecteddurationsecs seconds between calls to 'GetData', the internal buffer will overflow and you'll get "
    "scrambled sound! numchannels= Number of audio channels to use: 1=Mono recording, 2=Stereo recording. resolution = Size of "
    "a single sample: 1 = 1 Byte == 8 Bit resolution, 2 = 2 Bytes == 16 Bit resolution.";
    
    static char seeAlsoString[] = "StartRecording, GetData";

    ALboolean ext;
    
    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    // Child protection:
    PsychErrorExit(PsychCapNumInputArgs(4));   //The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(0)); //The required number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(1));  //The maximum number of outputs
    
    if (NULL!=in) {
        PsychErrorExitMsg(PsychError_user, "Tried to call InitRecording although sound capture is already initialized?!? Call ShutdownRecording first.");
    }

    // Check if OpenAL sound capture extension is available on this system:
    if (!alcIsExtensionPresent(NULL, "ALC_EXT_capture")) {
        PsychErrorExitMsg(PsychError_system, "OpenAL audio recording extension ALC_EXT_capture unavailable on this system. Sorry.");
    }
    
    // Resolve function pointers -- bind recording control extensions:
    #define GET_PROC(x) x = alcGetProcAddress(NULL, (ALubyte *) #x)
    GET_PROC(alcCaptureOpenDevice);
    GET_PROC(alcCaptureCloseDevice);
    GET_PROC(alcCaptureStart);
    GET_PROC(alcCaptureStop);
    GET_PROC(alcCaptureSamples);
    
    // These may not exist, depending on the implementation.
    _AL_FORMAT_MONO_FLOAT32 = alGetEnumValue((ALubyte *) "AL_FORMAT_MONO_FLOAT32");
    _AL_FORMAT_STEREO_FLOAT32 = alGetEnumValue((ALubyte *) "AL_FORMAT_STEREO_FLOAT32");
    alGetError();
    
    // Some debug output for the entertainment:
    #define printALCString(dev, ext) { ALenum e = alcGetEnumValue(dev, #ext); printf("%s: %s\n", #ext, alcGetString(dev, e)); }
    printALCString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
    
    // Some more output:
    ext = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if (!ext)
        printf("No ALC_ENUMERATION_EXT support.\n");
    else
    {
        char *devList;
        ALenum alenum = alcGetEnumValue(NULL, "ALC_CAPTURE_DEVICE_SPECIFIER");
        devList = (char *)alcGetString(NULL, alenum);
        
        printf("ALC_ENUMERATION_EXT:\n");
        while (*devList)  // I really hate this double null terminated list thing.
        {
            printf("  - %s\n", devList);
            devList += strlen(devList) + 1;
        } // while
    } // else
    
    fflush(NULL);
    
    // Try to open a sound capture device and retrieve handle to it:
    // For now the default system device with fixed sampling freq FREQ
    // in Mono-16 bit format with fixed sample buffer size.
    
    // This are our defaults: Mono recording with 16bps.
    channels = 1;
    resolution = 2;
    
    // Overrides from user?
    PsychCopyInIntegerArg(3, FALSE, &channels);
    if (channels < 1 || channels > 2) PsychErrorExitMsg(PsychError_user, "Number of channels needs to be either 1 or 2.");
    
    PsychCopyInIntegerArg(4, FALSE, &resolution);
    if (resolution < 1 || resolution > 2) PsychErrorExitMsg(PsychError_user, "Resolution needs to be either 1 or 2 bytes.");
    

    // Select sound capture format depending on req. settings:
    ALenum sound_format;
    if (channels==1 && resolution==1) sound_format = AL_FORMAT_MONO8;
    if (channels==1 && resolution==2) sound_format = AL_FORMAT_MONO16;
    if (channels==2 && resolution==1) sound_format = AL_FORMAT_STEREO8;
    if (channels==2 && resolution==2) sound_format = AL_FORMAT_STEREO16;

    // Retrieve requested samplingfreq. We default to 44100, if omitted.
    recording_freq = 44100;
    PsychCopyInIntegerArg(1, FALSE, &recording_freq);
    if (recording_freq < 8192) recording_freq = 8192;
    
    double lookaheadsecs = 5;
    PsychCopyInDoubleArg(2, FALSE, &lookaheadsecs);
    if (lookaheadsecs < 1) lookaheadsecs = 1;
    
    convbuffersize = (int) ((double) recording_freq * lookaheadsecs + 1);
    convbuffer = malloc(convbuffersize * channels * resolution * sizeof(unsigned char));
    if (convbuffer == NULL) {
        // Ooops...
        PsychErrorExitMsg(PsychError_outofMemory, "Couldn't allocate internal buffer memory due to out-of-memory condition.");
    }
    
    // Initialize ALUT toolkit with default settings:
    alutInit(0, NULL);
    
    in = alcCaptureOpenDevice(NULL, recording_freq, sound_format, convbuffersize);
    if (in == NULL) {
        // Ooops...
        free(convbuffer);
        convbuffersize = 0;
        alutExit();
        PsychErrorExitMsg(PsychError_system, "Couldn't open OpenAL audio capture device.");
    }
    
    // Bind the capture_samples enum code. We'll need it in the other routines:
    capture_samples = alcGetEnumValue(in, "ALC_CAPTURE_SAMPLES");

    // Initial recording state is "stopped" aka 0.
    recording_state = 0;

    // Return dummy-handle:
    double handle = 1;
    PsychCopyOutDoubleMatArg(1, TRUE, 1, 1, 1, &handle);

    return(PsychError_none);	
}

PsychError PSYCHSOUNDStartRecording(void)
{
    static char useString[] = "PsychSound('StartRecording', recorderPtr);"; 
    static char synopsisString[] = "Start continous recording of sound data. "
                             "recorderPtr=Handle to recording device. The sound hardware will perform "
                             "sound recording until you stop it again. You can fetch already recorded "
                             "data via use of 'GetData', at your choice during the ongoing recording or "
                             "at the end after you've stopped recording.";
    
    static char seeAlsoString[] = "StartRecording, GetData";

    // All subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    // Child protection:
    PsychErrorExit(PsychCapNumInputArgs(1));   //The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(1)); //The required number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs
    
    // Fetch device handle:
    int handleid = -1;
    PsychCopyInIntegerArg(1, TRUE, &handleid);

    if (NULL==in) {
        PsychErrorExitMsg(PsychError_user, "You need to call InitRecording once before calling StartRecording.");
    }

    if (recording_state!=0) {
        PsychErrorExitMsg(PsychError_user, "You tried to call StartRecording while recording is already active?!?");
    }

    // Reset counter for number of available samples in buffer:
    samples = 0;
    
    // Start sound capture:
    alcCaptureStart(in);
    recording_state = 1;
    
    return(PsychError_none);	
}

PsychError PSYCHSOUNDStopRecording(void)
{
    static char useString[] = "PsychSound('StopRecording', recorderPtr);"; 
    static char synopsisString[] = "Stop recording of sound data. "
                             "recorderPtr=Handle to recording device. ";
    static char seeAlsoString[] = "StartRecording, GetData";

    // All subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    // Child protection:
    PsychErrorExit(PsychCapNumInputArgs(1));   //The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(1)); //The required number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs
    
    // Fetch device handle:
    int handleid = -1;
    PsychCopyInIntegerArg(1, TRUE, &handleid);

    if (NULL==in) {
        PsychErrorExitMsg(PsychError_user, "You need to call InitRecording once before calling StopRecording.");
    }
    
    if (recording_state!=1) {
        PsychErrorExitMsg(PsychError_user, "You tried to call StopRecording while recording is already stopped?!?");
    }
    
    // Stop recording:
    alcCaptureStop(in);
    recording_state = 0;
    
    return(PsychError_none);	
}

PsychError PSYCHSOUNDShutdownRecording(void)
{
    static char useString[] = "PsychSound('ShutdownRecording', recorderPtr);"; 
    static char synopsisString[] = "Close recording device, release all internal ressources. "
                             "recorderPtr=Handle to recording device.";
    static char seeAlsoString[] = "";
    
    // All subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    // Child protection:
    PsychErrorExit(PsychCapNumInputArgs(1));   //The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(1)); //The required number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs
    
    // Fetch device handle:
    int handleid = -1;
    PsychCopyInIntegerArg(1, TRUE, &handleid);

    if (NULL==in) {
        PsychErrorExitMsg(PsychError_user, "You need to call InitRecording once before calling ShutdownRecording.");
    }

    PsychSoundExit();

    // Ready.
    return(PsychError_none);
}

PsychError PsychSoundExit(void)
{
    // Release internal buffer:
    if (convbuffer) free(convbuffer);
    convbuffer = NULL;
    convbuffersize = 0;
    recording_state = 0;
    
    // Stop & Close device:
    if (in!=NULL) {
        alcCaptureStop(in);
        recording_state = 0;
        alcCaptureCloseDevice(in);
        in = NULL;
        // Close toolkit.
        alutExit();
    }
    
    printf("PsychSound jettisoned...\n");
    fflush(NULL);

    return(PsychError_none);
}

/*
void blah(void)
{
    alGenSources(1, &sid);
alGenBuffers(1, &bid);

printf("Playing...\n");

alBufferData(bid, AL_FORMAT_MONO16, buf, sizeof (buf), FREQ);
alSourcei(sid, AL_BUFFER, bid);
alSourcePlay(sid);

while (state == AL_PLAYING)
{
    usleep(100000);
    alGetSourcei(sid, AL_SOURCE_STATE, &state);
} // while

printf("Cleaning up...\n");

alDeleteSources(1, &sid);
alDeleteBuffers(1, &bid);

alutExit();
return(PsychError_none);	
}
*/

PsychError PSYCHSOUNDGetRecordingPosition(void)
{
    static char useString[] = "nrsamplesavailable = PsychSound('GetRecordingPosition', recorderPtr);"; 
    static char synopsisString[] = "Return number of samples that are currently available in internal soundbuffer. "
    "recorderPtr=Handle to recording device.";
    static char seeAlsoString[] = "GetData";

    // All subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    // Child protection:
    PsychErrorExit(PsychCapNumInputArgs(1));   //The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(1)); //The required number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(1));  //The maximum number of outputs
    
    if (NULL==in) {
        PsychErrorExitMsg(PsychError_user, "You need to call InitRecording once before calling GetRecordingPosition.");
    }

    // Fetch device handle:
    int handleid = -1;
    PsychCopyInIntegerArg(1, TRUE, &handleid);

    int samples=0;
    double nrsamples;

    // Query current fill level of ringbuffer:
    alcGetIntegerv(in, capture_samples, sizeof (samples), &samples);
    nrsamples = (double) samples;
    
    PsychCopyOutDoubleMatArg(1, TRUE, 1, 1, 1, &nrsamples);
    
    return(PsychError_none);	
}

PsychError PSYCHSOUNDGetData(void)
{
    static char useString[] = "samplebuffer = PsychSound('GetData', recorderPtr [, nrsamples=inf] [, sync=0] [, pollintervals=0.01]);"; 
    static char synopsisString[] = "Fetch recorded sound samples from internal soundbuffer and return them in a Matlab matrix. "
                             "recorderPtr=Handle to recording device. nrsamples = Number of samples to fetch. Fetches at "
                             "most the given amount. If nrsamples is left out, all available samples will be returned. sync = If "
                             "set to 1 then PTB will wait for the requested number of samples to become available. Otherwise "
                             "it will just return the amount of available samples, up to nrsamples. pollintervals = If sync==1 "
                             "this determines the waiting time before retrying a fetch in seconds.";
    static char seeAlsoString[] = "GetRecordingPosition";
    
    // All subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    // Child protection:
    PsychErrorExit(PsychCapNumInputArgs(4));   //The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(1)); //The required number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(1));  //The maximum number of outputs
    
    if (NULL==in) {
        PsychErrorExitMsg(PsychError_user, "You need to call InitRecording once before calling GetData.");
    }

    // Fetch device handle:
    int handleid = -1;
    PsychCopyInIntegerArg(1, TRUE, &handleid);

    // Fetch optional nrsamples to return, defaults to infinity.
    int nrsamples = 0;
    PsychCopyInIntegerArg(2, FALSE, &nrsamples);
    if (nrsamples > convbuffersize) nrsamples = convbuffersize;
    
    // Fetch optional syncflag, defaults to async, aka 0.
    int syncflag = 0;
    PsychCopyInIntegerArg(3, FALSE, &syncflag);
    
    // Fetch optional delay, defaults to 10 ms.
    double polldelay = 0.01;
    PsychCopyInDoubleArg(4, FALSE, &polldelay);
    if (polldelay < 0.001) polldelay = 0.001;
    
    // Query current fill level of ringbuffer:
    alcGetIntegerv(in, capture_samples, sizeof (samples), &samples);

    // nrsamples spec'd?
    if (nrsamples > 0) {
        // User specified a value. Are we in sync-mode?
        if (syncflag>0) {
            // User wants us to wait for the requested amount to become
            // available. We wait in a loop until requested amount is
            // available or recording is stopped - in which case we'll
            // never get what we want.
            while ((samples < nrsamples) && (recording_state > 0))
            {
                //PsychWaitIntervalSeconds(polldelay);
                usleep((int) (polldelay * 1000 * 1000));
                alcGetIntegerv(in, capture_samples, sizeof (samples), &samples);                
            }
            // Either we have the reqested amount, or we have all we can get,
            // given that capture is stopped and therefore no new date will arrive.
        }

        // Specified value is (also) an upper limit. We fetch what we
        // can get, but at most given amount.
        samples = (samples > nrsamples) ? nrsamples : samples;
    }

    // Ok, we want to capture "samples" samples. Allocate appropriate output buffer...
    
    // Allocate output double type buffer:
    if (samples > convbuffersize) {
        // Realloc the conversionbuffer of appropriate size:
        convbuffersize = samples;
        convbuffer = realloc(convbuffer, convbuffersize * sizeof(unsigned char) * resolution * channels);
        if (convbuffer==NULL) {
            // Realloc failure???
            // FIXME: We leak memory here in case of failure!
            convbuffersize = 0;
            PsychErrorExitMsg(PsychError_outofMemory, "Internal convbuffer-memory allocation failed due to out-of-memory condition!!! Aborted."); 
        }
    }

    // Ok, fetch the data:
    if (samples > 0) {
        alcCaptureSamples(in, convbuffer, samples);
    }
    else {
        // Zero samples requested? We return one dummy sample so we can return an
        // output matrix at all...
        samples = 1;
    }

    // Now we need to convert into the fu**ed up Matlab format double with range -1.0 to 1.0
    double* outmatrix = NULL;
    PsychAllocOutDoubleMatArg(1, kPsychArgRequired, channels, samples, 1, (double**) &outmatrix);
    if (outmatrix==NULL) {
        // malloc failure???
        PsychErrorExitMsg(PsychError_outofMemory, "Internal outmatrix-memory allocation failed due to out-of-memory condition!!! Aborted."); 
    }
    
    // Perform data conversion:
    int i;

    if (resolution==2) {
        // 16 bits per sample, aka 2 Bytes per sample, aka shortint resolution:
        short int* bufptr = (short int*) convbuffer;
        int count = samples * channels;
        for (i=0; i<count; i++) {
            *(outmatrix++) = ((double) bufptr[i]) / 32768;
        }
    }
    else {
        // 8 bits per sample, aka 1 Byte per sample, aka uint8 resolution:
        char* bufptr = (char*) convbuffer;
        int count = samples * channels;
        for (i=0; i<count; i++) {
            *(outmatrix++) = ((double) bufptr[i]) / 128;
        }        
    }
    
    // Ready.
    return(PsychError_none);	
}

