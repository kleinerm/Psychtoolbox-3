/*
	SCREENFlip.c		
  
	AUTHORS:

		Allen.Ingling@nyu.edu               awi
                mario.kleiner at tuebingen.mpg.de   mk
  
	PLATFORMS:	
	
		OS X only for now.
    

	HISTORY:

		09/10/02	awi		Created.
		10/12/04	awi		Changed "SCREEN" to "Screen" in useString.
                04/03/05        mk              Add optional sync/nosync to VBL, don't clear fb on flip, flip after deadline, and return timestamps.
                05/16/05        mk              Add optional flag "dontsync" and some more timestamps.
                06/09/05        mk              Add optional flag "multiflip" for experimental multiflip support.
 
	DESCRIPTION:
  
		Interchanges front and back buffers of a double-buffered display.
                It can be selected if buffer should be cleared to background color after the flip or if
                it is left intact. One can also select to flip after a specific deadline -
                useful for easier timing of stimulus presentation. And Flip returns multiple timestamps
                that help to check proper presentation timing.

	TO DO:
  
		This should do some checks and exit with an error if conditions are not met:
			1- An onscreen window should be open
			2- It should have two surfaces
  

*/


#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "[VBLTimestamp StimulusOnsetTime FlipTimestamp Missed Beampos] = Screen('Flip', windowPtr [, when] [, dontclear] [, dontsync] [, multiflip]);";
static char synopsisString[] = 
	"Flip front and back display surfaces in sync with vertical retrace and return timestamps. "
        "\"windowPtr\" is the id of the onscreen window whose content should be shown at flip time. "
        "\"when\" specifies when to flip: If set to zero (default), it will flip on the next possible retrace."
        "If set to a value when > 0, it will flip at the first retrace after system time 'when' has been reached."
        "\"dontclear\" If set to 1, flip will not clear the framebuffer after Flip - this allows incremental drawing "
        "of stimuli. The default is zero, which will clear the framebuffer to background color after each flip. "
        "A value of 2 will prevent Flip from doing anything to the framebuffer after flip. This leaves the job "
        "of setting up the buffer to you - the framebuffer is in an undefined state after flip. "
        "\"dontsync\" If set to zero (default), Flip will sync to the vertical retrace and will pause Matlabs execution"
        "until the Flip has happened. If set to 1, Flip will still synchronize stimulus onset to the vertical retrace, but"
        "will *not* wait for the flip to happen: Flip returns immediately and all returned timestamps are invalid. A value of 2"
        "will cause Flip to show the stimulus *immediately* without waiting/syncing to the vertical retrace. "
        "\"multiflip\" defaults to zero: If set to a value greater than zero, Flip will flip *all* onscreen windows instead of "
        "just the specified one. This allows to synchronize stimulus onset on multiple displays, e.g., for multidisplay stereo "
        "setups or haploscopes. You need to (somehow) synchronize all attached displays for this to operate tear-free. "
        "Flip (optionally) returns a high-precision estimate of the system time (in seconds) when the actual flip has happened "
        "in the return argument 'VBLTimestamp'. An estimate of Stimulus-onset time is returned in 'StimulusOnsetTime'. "
        "Beampos is the position of the monitor scanning beam when the time measurement was taken (useful for correctness tests). " 
        "FlipTimestamp is a timestamp taken at the end of Flip's execution. Use the difference between FlipTimestamp and VBLTimestamp "
        "to get an estimate of how long Flips execution takes. This is useful to get a feeling for the timing error if you try to sync "
        "Matlabs execution to the retrace, e.g., for triggering acquisition devices like EEG, fMRI, or for starting playback of a sound. "
        "\"Missed\" indicates if the requested presentation deadline for your stimulus has been missed. A negative value means that dead- "
        "lines have been satisfied. Positive values indicate a deadline-miss. The automatic detection of deadline-miss is not fool-proof"
        " - it can report false positives and also false negatives, although it should work fairly well with most experimental setups. "
        "If you are picky about timing, please use the provided timestamps or additional methods to exercise your own tests.";

static char seeAlsoString[] = "DrawingFinished";

PsychError SCREENFlip(void) 
{
	PsychWindowRecordType *windowRecord;
        int dont_clear;
        int vbl_synclevel;
        int multiflip;
        double flipwhen;
        double vbl_timestamp;
        int beamposatflip;
        double miss_estimate;
        double time_at_flipend;
        double time_at_onset;
        
	//all subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	PsychErrorExit(PsychCapNumInputArgs(5));   //The maximum number of inputs
        PsychErrorExit(PsychRequireNumInputArgs(1)); //The required number of inputs
        PsychErrorExit(PsychCapNumOutputArgs(5));  //The maximum number of outputs
        
	//get the window record from the window record argument and get info from the window record
	PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);

	if(windowRecord->windowType!=kPsychDoubleBufferOnscreen)
		PsychErrorExitMsg(PsychError_user, "Flip called on window without backbuffers. Specify numberOfBuffers=2 in Screen('OpenWindow') if you want to use Flip.");

        // Query optional dont_clear argument: 0 (default) clear backbuffer to background color after flip.
        // 1 == Restore backbuffer to state before flip - this allows incremental drawing/updating of stims.
        dont_clear=0;
        PsychCopyInIntegerArg(3,FALSE,&dont_clear);
        if(dont_clear < 0 || dont_clear > 2)
            PsychErrorExitMsg(PsychError_user, "Only 'dontclear' values 0 (== clear after flip), 1 (== don't clear) and 2 (== don't do anything) are supported");
        
        // Query optional vbl_synclevel argument: 0 (default) Synchronize flip to VBL and wait (block) for VBL onset.
        // 1 == Synchronize flip to VBL, but don' wait for flip to happen -- timestamps are invalid.
        // 2 == Don't sync to the retrace *at all* Flip as soon as stimulus is ready.
        // EXPERIMENTAL modes:
        // 3 == Like 0, but use spin-waiting to busy wait for VBL onset -> Non-blocking -> Low delay -> But dangerous!
        vbl_synclevel=0;
        PsychCopyInIntegerArg(4,FALSE,&vbl_synclevel);
        if(vbl_synclevel < 0 || vbl_synclevel > 3)
            PsychErrorExitMsg(PsychError_user, "Only 'dontsync' values 0 (== fully synchronize with VBL), 1 (== don't wait for VBL) and 2 (== Ignore VBL) are supported");
        
        // Query optional multiflip argument: 0 (default) Only flip the requested onscreen window.
        // 1 = Flip *all* onscreen windows simultaneously, but individually synced to their respective VBL onset.
        // -> This is tear- and flicker-free and ressource efficient due to its blocking nature, but it requires
        // displays that retrace in sync, otherwise stimulus onset on each display can happen phase-shifted to other
        // displays.
        // 2 = Like 1, but we wait for VBL onset on requested onscreen window and then immediately flip all other
        // windows without waiting for their respective retrace -> All stimuli show up at the same time/with minimal
        // time delay, but if monitors are not in-phase, some of them will tear/flicker.
        multiflip=0;
        PsychCopyInIntegerArg(5,FALSE,&multiflip);
        if(multiflip < 0 || multiflip > 2)
            PsychErrorExitMsg(PsychError_user, "Only 'multiflip' values 0 (== don't multiflip), 1 (== multiflip method 1) and 2 (== multiflip method 2) are supported");
        if (multiflip==2 && vbl_synclevel!=0 && vbl_synclevel!=3) {
            PsychErrorExitMsg(PsychError_user, "Only 'dontsync' values 0 and 3 are allowed when multiflip is set to 2.");
        }
        
        // Query optional flipwhen argument: -1 == Don't sync to vertical retrace --> Only useful for debugging!
        // Use this only if you "really know what you're doing(TM)!"
        // 0 (default value) == Flip at next vertical retrace and sync to VBL. This is the old PTB behaviour as of PTB 1.0.42.
        // flipwhen>0 == Sleep/Wait until system time "flipwhen" and then flip at the next VBL after time "flipwhen".
        flipwhen=0;
        PsychCopyInDoubleArg(2,FALSE,&flipwhen);
        if(flipwhen < 0)
            PsychErrorExitMsg(PsychError_user, "Only 'when' values greater or equal to 0 are supported");
        
        // Issue the flip-request:
        vbl_timestamp = PsychFlipWindowBuffers(windowRecord, multiflip, vbl_synclevel, dont_clear, flipwhen, &beamposatflip, &miss_estimate, &time_at_flipend, &time_at_onset);
        
        // Return timestamp at start of VBL time:
        PsychCopyOutDoubleArg(1, FALSE, vbl_timestamp);
        // Return timestamp at stimulus onset time:
        PsychCopyOutDoubleArg(2, FALSE, time_at_onset);
        // Return time when Flip ended:
        PsychCopyOutDoubleArg(3, FALSE, time_at_flipend);
        // Return current estimate of deadline miss, if any:
        PsychCopyOutDoubleArg(4, FALSE, miss_estimate);
        // Return beam position at VBL time:
        PsychCopyOutDoubleArg(5, FALSE, (double) beamposatflip);
        
	return(PsychError_none);
	
}


	
	






