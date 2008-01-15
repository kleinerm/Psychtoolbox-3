/*
	SCREENFlip.c		
  
	AUTHORS:

		Allen.Ingling@nyu.edu               awi
		mario.kleiner at tuebingen.mpg.de   mk
  
	PLATFORMS:	
	
		All.

	HISTORY:

		09/10/02	awi		Created.
		10/12/04	awi		Changed "SCREEN" to "Screen" in useString.
		04/03/05    mk      Add optional sync/nosync to VBL, don't clear fb on flip, flip after deadline, and return timestamps.
		05/16/05	mk      Add optional flag "dontsync" and some more timestamps.
		06/09/05	mk      Add optional flag "multiflip" for experimental multiflip support.
 
	DESCRIPTION:
  
		Interchanges front and back buffers of a double-buffered display.
		It can be selected if buffer should be cleared to background color after the flip or if
		it is left intact. One can also select to flip after a specific deadline -
		useful for easier timing of stimulus presentation. And Flip returns multiple timestamps
		that help to check proper presentation timing.

	TO DO:
  
*/

#include "Screen.h"

// SCREENFlip internally implements both Screen('Flip') and Screen('FlipAsyncBegin') and Screen('FlipAsyncEnd'):
PsychError SCREENFlip(void) 
{
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

	PsychFlipInfoStruct*  flipRequest;
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
	unsigned int opmode;
	
	// Change our "personality" depending on the name with which we were called:
	if (strstr(PsychGetFunctionName(), "FlipAsyncBegin")) {
		// Async flip invocation:
		opmode = 1;
	}
	else if (strstr(PsychGetFunctionName(), "FlipAsyncEnd")) {
		// Finalize async flip operation:
		opmode = 2;
	}
	else if (strstr(PsychGetFunctionName(), "FlipAsyncCheckEnd")) {
		// Poll for finalization of async flip operation:
		opmode = 3;
	}
	else {
		// Standard synchronous flip:
		opmode = 0;
	};

	// All subfunctions should have these two lines.
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	PsychErrorExit(PsychCapNumInputArgs((opmode != 2)  ? 5 : 1));		// The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1));						// The required number of inputs
	PsychErrorExit(PsychCapNumOutputArgs((opmode != 1) ? 5 : 0));		// The maximum number of outputs
	
	// Get the window record from the window record argument and get info from the window record
	PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
	
	if (!PsychIsOnscreenWindow(windowRecord)) PsychErrorExitMsg(PsychError_user, "Flip called on something else than an onscreen window. You can only flip onscreen windows.");
	if(windowRecord->windowType!=kPsychDoubleBufferOnscreen) PsychErrorExitMsg(PsychError_user, "Flip called on window without backbuffers. Specify numberOfBuffers=2 in Screen('OpenWindow') if you want to use Flip.");
	
	// Only retrieve additional arguments if this isn't a finish on an async flip:
	if ((opmode != 2) && (opmode != 3)) {
		// Query optional dont_clear argument: 0 (default) clear backbuffer to background color after flip.
		// 1 == Restore backbuffer to state before flip - this allows incremental drawing/updating of stims.
		dont_clear=0;
		PsychCopyInIntegerArg(3,FALSE,&dont_clear);
		if(dont_clear < 0 || dont_clear > 2) {
			PsychErrorExitMsg(PsychError_user, "Only 'dontclear' values 0 (== clear after flip), 1 (== don't clear) and 2 (== don't do anything) are supported");
		}
		
		// Query optional vbl_synclevel argument: 0 (default) Synchronize flip to VBL and wait (block) for VBL onset.
		// 1 == Synchronize flip to VBL, but don' wait for flip to happen -- timestamps are invalid.
		// 2 == Don't sync to the retrace *at all* Flip as soon as stimulus is ready.
		// EXPERIMENTAL modes:
		// 3 == Like 0, but use spin-waiting to busy wait for VBL onset -> Non-blocking -> Low delay -> But dangerous!
		vbl_synclevel=0;
		PsychCopyInIntegerArg(4,FALSE,&vbl_synclevel);
		if(vbl_synclevel < 0 || vbl_synclevel > 3) {
			PsychErrorExitMsg(PsychError_user, "Only 'dontsync' values 0 (== fully synchronize with VBL), 1 (== don't wait for VBL) and 2 (== Ignore VBL) are supported");
		}
		
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
		if(flipwhen < 0) {
			PsychErrorExitMsg(PsychError_user, "Only 'when' values greater or equal to 0 are supported");
		}
		
		// Old implementation - DISABLED: Issue the flip-request:
		if (FALSE) {
			vbl_timestamp = PsychFlipWindowBuffers(windowRecord, multiflip, vbl_synclevel, dont_clear, flipwhen, &beamposatflip, &miss_estimate, &time_at_flipend, &time_at_onset);
		}

		// Pack all parameters of the fliprequest into the flipinfo struct:
		if (NULL == windowRecord->flipInfo) {
			// First time invocation: Alloc and clear-init the struct...
			windowRecord->flipInfo = (PsychFlipInfoStruct*) malloc(sizeof(PsychFlipInfoStruct));
			memset(windowRecord->flipInfo, 0, sizeof(PsychFlipInfoStruct));
			// printf("FT\n");
		}

		// Fill it: At least our part. Initial setup of threds and locks etc. is done by the actual
		// PsychFlipWindowBuffersIndirect() routine:
		flipRequest = windowRecord->flipInfo;
		// printf("FT opmode = %i , flR %p, %ld\n", opmode, flipRequest, flipRequest->asyncstate);

		if (flipRequest->asyncstate != 0) {
			// Started, executing or finalized async flip in progress! We're not idle, so we can't
			// trigger a new flip request!
			PsychErrorExitMsg(PsychError_user, "Screen('Flip'); or Screen('FlipAsyncBegin'); called while a scheduled flip operation is still in progress! Forbidden! Check your code!!");			
		}

		flipRequest->opmode			= opmode;
		flipRequest->dont_clear		= dont_clear;
		flipRequest->flipwhen		= flipwhen;
		flipRequest->multiflip		= multiflip;
		flipRequest->vbl_synclevel	= vbl_synclevel;		

		// Ok, the struct is filled with spec for a synchronous or asynchronous flip...
	}
	else {
		// opmode == 2 or 3
		// 'FlipAsyncEnd' or 'FlipAsyncPoll': Just make sure the flip info record is actually allocated:
		if (NULL == windowRecord->flipInfo) {
			// Flip end called without any flipInfo struct allocated?!? A flip op wasn't ever triggered!
			PsychErrorExitMsg(PsychError_user, "Screen('FlipAsyncEnd'); called without calling Screen('FlipAsyncBegin') before to actually start an async Flip! Forbidden! Check your code!!");
		}

		flipRequest = windowRecord->flipInfo;
		if (flipRequest->asyncstate == 0) {
			// No started, executing or finalized async flip in progress! No async flip operation triggered
			// which we could finalize!
			PsychErrorExitMsg(PsychError_user, "Screen('FlipAsyncEnd'); called without calling Screen('FlipAsyncBegin') before to actually start an async Flip! Forbidden! Check your code!!");			
		}

		// Ok, there's an async flip going on and we have a handle to it...
		flipRequest->opmode	= opmode;
	}

	// Execute the flip request with the indirect routine: In opmode == 0 (Synchronous flip)
	// this will block until flip is done, then return with the results. In async mode it
	// will dispatch the flip to the helper thread, then return immediately. In async end
	// or async poll mode, it will either block until async op finished, or return after
	// polling with a FALSE result, telling that not yet finished:
	if (!PsychFlipWindowBuffersIndirect(windowRecord)) return(PsychError_none);	// Polling mode and polling says "Not yet done" - Just return.
	
	// Only have return args in synchronous mode or in return path from end/successfull poll of async flip:
	if (opmode != 1) {
		// Async flip is either zero in synchronous mode, or its 2 if an async flip
		// successfully finished:
		if ((flipRequest->asyncstate!=0) && (flipRequest->asyncstate!=2)) {
			printf("PTB-ERROR: flipRequest->asyncState has impossible value %i at end of flipop! This is a PTB DESIGN BUG!", flipRequest->asyncstate);
			PsychErrorExitMsg(PsychError_internal, "flipRequest->asyncState has impossible value at end of flipop! This is a PTB DESIGN BUG!");
		}
		
		// Reset it to zero, ie. ready for new adventures ;-)
		flipRequest->asyncstate=0;
		
		// Return return arguments from flip:
		vbl_timestamp	= flipRequest->vbl_timestamp;
		time_at_onset	= flipRequest->time_at_onset;
		time_at_flipend = flipRequest->time_at_flipend;
		miss_estimate	= flipRequest->miss_estimate;
		beamposatflip	= flipRequest->beamPosAtFlip;
		
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
	}
	
	return(PsychError_none);	
}
