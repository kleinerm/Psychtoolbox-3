/*
 *
 * alm.c -- implementation of alm functions
 *
 * 07-Feb-2007 -- created (MK)
 * 27-Mar-2011 -- Bug fixes and cleanup (MK)
 */

#include "moaltypes.h"

// Audio device handle:
static ALCdevice* device = NULL;

// Audio context handle:
static ALCcontext* context = NULL;

// general-purpose text buffer
#define TEXT_SIZE 4096
char text[TEXT_SIZE];

void glm_doclose(void)
{
    // Child-protection:
    if (context) {
        // Tear down context
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        context = NULL;
    }
    
    // Shutdown audio device:
    if (device) alcCloseDevice(device);
    device = NULL;
    
    return;
}

void glm_close(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

    // Call our shutdown routine:
    glm_doclose();
    return;
}

void glm_open(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

    ALCenum alcerr;
    static int firsttime = 1;
    char devicename[1000];
    int attribs[] = {0, 0, 0, 0, 0, 0};
        
    if (device) mexErrMsgTxt("MOAL-ERROR: Tried to open sound device, but device already open!\n");
    
    // See whether there's a sound device spec. string:
    if(nrhs<1 || !mxIsChar(prhs[0])) {
        // Nope. Choose default device:
        devicename[0] = 0;
    }
    else {
        // Get device specifier string:
        mxGetString(prhs[0], devicename, 999);
    }

    
    // Open requested device:
    device = alcOpenDevice((devicename[0]!=0) ? devicename : NULL);
    if (device == NULL) mexErrMsgTxt("MOAL-ERROR: Could not open requested sound device, unknown error!\n");

    // Only for testing of low-latency, didn't change anything: alcMacOSXMixerMaxiumumBusses(1);

    // Attributes provided?
    if (nrhs>1) {
        // First attribute parameter would be refresh rate, whatever that means...
        attribs[0]=ALC_REFRESH;
        attribs[1]=(int) mxGetScalar(prhs[1]);
        printf("MOAL: ALC_REFRESH is %i\n", attribs[1]);
    }
    
    if (nrhs>2) {
        // 2nd attribute parameter would be native device sampling rate.
        attribs[2]=ALC_FREQUENCY;
        attribs[3]=(int) mxGetScalar(prhs[2]);
        printf("MOAL: ALC_FREQUENCY is %i\n", attribs[3]);
    }

    // Create OpenAL audio context and attach it to device:
    context = alcCreateContext(device, (attribs[0]) ? attribs : NULL);
    alcerr = alcGetError(device);
    if (alcerr) {
        printf("MOAL-ERROR[alcCreateContext()]: Audio context creation failed [errorcode %i], ALC says: %s\n", (int) alcerr, (alcerr==ALC_INVALID_DEVICE) ? "Invalid output device." : "Out of ressources for further contexts.");
    }

    if (context == NULL) mexErrMsgTxt("MOAL-ERROR: Could not create OpenAL context for requested sound device, unknown error!\n");

    // Activate audio rendering context:
    alcMakeContextCurrent(context);
	alcerr = alcGetError(device);
    if (alcerr) {
		printf("MOAL-ERROR: Could not activate OpenAL context for requested sound device [errorcode %i]. ALC says: Invalid context!\n", (int) alcerr);
		mexErrMsgTxt("MOAL-ERROR: Could not activate OpenAL context for requested sound device.\n");
    }
	
    // Clear context error state:
    alGetError();
    
    #ifdef MACOSX
    // OS-X specific use of extensions:
    
    // Enforce all data conversion at buffer load time, to reduce delays during playback:
    alEnable(ALC_MAC_OSX_CONVERT_DATA_UPON_LOADING);
    
    // Enable high quality mode for spatial 3D audio:
    alcMacOSXRenderingQuality(ALC_MAC_OSX_SPATIAL_RENDERING_QUALITY_HIGH);
        
    #endif
    
    // Clear context error state:
    alGetError();

    return;
}

void glm_suspend(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

    if (context == NULL) mexErrMsgTxt("MOAL-ERROR: Tried to suspend context, but no context exists?!?\n");

    // Suspend audio rendering context:
    alcSuspendContext(context);
   
    return;
}

void glm_process(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

    if (context == NULL) mexErrMsgTxt("MOAL-ERROR: Tried to restart context processing, but no context exists?!?\n");

    // Restart state processing for audio rendering context:
    alcProcessContext(context);
   
    return;
}

// command map:  moalcore string commands and functions that handle them
// *** it's important that this list be kept in alphabetical order, 
//     and that glm_map_count be updated for 
//     each new entry ***
int glm_map_count=4;
cmdhandler glm_map[] = {
{ "almClose",                       glm_close                           },
{ "almOpen",                        glm_open                            },
{ "almProcessContext",              glm_process                         },
{ "almSuspendContext",              glm_suspend                         }};
