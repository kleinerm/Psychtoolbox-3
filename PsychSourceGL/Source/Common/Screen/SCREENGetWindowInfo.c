/*
  SCREENGetWindowInfo.c		
  
  AUTHORS:

  mario.kleiner at tuebingen.mpg.de  mk
  
  PLATFORMS:	All

  HISTORY:
  06/03/07  mk		Created.
 
  DESCRIPTION:
  
  Returns all kind of misc information about a specific window in a struct.
  This is a catch-all for information that's not of too much interest for regular
  users, but useful for Psychtoolbox helper functions (M-Files).
  
*/

#include "Screen.h"

static char useString[] = "info = Screen('GetWindowInfo', windowPtr [, beamposOnly=0]);";
static char synopsisString[] = 
	"Returns a struct with miscellaneous info for the specified onscreen window."
	"\"windowPtr\" is the handle of the onscreen window for which info should be returned. "
	"\"beamposOnly\" If left out or set to zero, all available information is returned. "
	"If set to one, only the rasterbeam position is returned (or -1 if unsupported).\n"
	"The info struct contains all kinds of information. Just check its output to see what "
	"is returned. Most of this info is not interesting for normal users, mostly provided "
	"for internal use by M-Files belonging to Psychtoolbox itself, e.g., display tests.\n";

static char seeAlsoString[] = "OpenWindow, Flip, NominalFrameRate";
	 
PsychError SCREENGetWindowInfo(void) 
{
    const char *FieldNames[]={ "Beamposition", "LastVBLTimeOfFlip", "LastVBLTime", "VBLCount", "StereoMode", "ImagingMode", "MultiSampling", "MissedDeadlines", "StereoDrawBuffer",
							   "GuesstimatedMemoryUsageMB", "VBLStartline", "VBLEndline", "VideoRefreshFromBeamposition", "GLVendor", "GLRenderer", "GLVersion"};
	const int  fieldCount = 16;
	PsychGenericScriptType	*s;
	
    PsychWindowRecordType *windowRecord;
    double beamposition, lastvbl;
	int beamposonly = 0;
	CGDirectDisplayID displayId;
	psych_uint64 postflip_vblcount;
	double vbl_startline;
	long scw, sch;
	bool onscreen;
	
    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    PsychErrorExit(PsychCapNumInputArgs(2));     //The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(1)); //The required number of inputs	
    PsychErrorExit(PsychCapNumOutputArgs(1));    //The maximum number of outputs
    
    // Get the window record:
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
	onscreen = PsychIsOnscreenWindow(windowRecord);
    
    // Query beamposonly flag: Defaults to zero.
    PsychCopyInIntegerArg(2, FALSE, &beamposonly);

	if (onscreen) {
		// Query rasterbeam position: Will return -1 if unsupported.
		PsychGetCGDisplayIDFromScreenNumber(&displayId, windowRecord->screenNumber);
		beamposition = (double) PsychGetDisplayBeamPosition(displayId, windowRecord->screenNumber);
	}
	else {
		beamposition = -1;
	}
	
	if (beamposonly) {
		// Return the measured beamposition:
		PsychCopyOutDoubleArg(1, FALSE, beamposition);
	}
	else {
		// Return all information:
		PsychAllocOutStructArray(1, FALSE, 1, fieldCount, FieldNames, &s);

		// Rasterbeam position:
		PsychSetStructArrayDoubleElement("Beamposition", 0, beamposition, s);

		// Time of last vertical blank when a double-buffer swap occured:
		PsychSetStructArrayDoubleElement("LastVBLTimeOfFlip", 0, windowRecord->time_at_last_vbl, s);

		// Try to determine system time of last VBL on display, independent of any
		// flips / bufferswaps.
		lastvbl = -1;
		postflip_vblcount = 0;
		
		#if PSYCH_SYSTEM == PSYCH_OSX
			// On OS/X we can query the OS for the system time of last VBL, so we can
			// use the most recent VBL timestamp as baseline for timing calculations, 
			// instead of one far in the past.
			if (onscreen) { lastvbl = PsychOSGetVBLTimeAndCount(windowRecord->screenNumber, &postflip_vblcount); }
		#endif

		// If we couldn't determine this information we just set lastvbl to the last known
		// vbl timestamp of last flip -- better than nothing...
		if (lastvbl < 0) lastvbl = windowRecord->time_at_last_vbl;
		PsychSetStructArrayDoubleElement("LastVBLTime", 0, lastvbl, s);
		PsychSetStructArrayDoubleElement("VBLCount", 0, (double) postflip_vblcount, s);

		// Misc. window parameters:
		PsychSetStructArrayDoubleElement("StereoMode", 0, windowRecord->stereomode, s);
		PsychSetStructArrayDoubleElement("ImagingMode", 0, windowRecord->imagingMode, s);
		PsychSetStructArrayDoubleElement("MultiSampling", 0, windowRecord->multiSample, s);
		PsychSetStructArrayDoubleElement("MissedDeadlines", 0, windowRecord->nr_missed_deadlines, s);
		PsychSetStructArrayDoubleElement("StereoDrawBuffer", 0, windowRecord->stereodrawbuffer, s);
		PsychSetStructArrayDoubleElement("GuesstimatedMemoryUsageMB", 0, (double) windowRecord->surfaceSizeBytes / 1024 / 1024, s);
		
		// Query real size of the underlying display in order to define the vbl_startline:
		PsychGetScreenSize(windowRecord->screenNumber, &scw, &sch);
		vbl_startline = (double) sch;
		PsychSetStructArrayDoubleElement("VBLStartline", 0, vbl_startline, s);

		// And VBL endline:
		PsychSetStructArrayDoubleElement("VBLEndline", 0, windowRecord->VBL_Endline, s);

		// Video refresh interval duration from beamposition method:
		PsychSetStructArrayDoubleElement("VideoRefreshFromBeamposition", 0, windowRecord->ifi_beamestimate, s);
		
		// Renderer information:
		PsychSetGLContext(windowRecord);
		PsychSetStructArrayStringElement("GLVendor", 0, glGetString(GL_VENDOR), s);
		PsychSetStructArrayStringElement("GLRenderer", 0, glGetString(GL_RENDERER), s);
		PsychSetStructArrayStringElement("GLVersion", 0, glGetString(GL_VERSION), s);
	}
	
    // Done.
    return(PsychError_none);
}
