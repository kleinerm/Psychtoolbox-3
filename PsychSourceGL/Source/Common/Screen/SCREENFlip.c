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
	static char useString0[] = "[VBLTimestamp StimulusOnsetTime FlipTimestamp Missed Beampos] = Screen('Flip', windowPtr [, when] [, dontclear] [, dontsync] [, multiflip]);";
	static char synopsisString0[] = 
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

	static char useString1[] = "Screen('AsyncFlipBegin', windowPtr [, when] [, dontclear] [, dontsync] [, multiflip]);";
	static char synopsisString1[] = 
	"Schedule an asynchronous flip of front and back display surfaces for given onscreen window. "
	"\"windowPtr\" is the id of the onscreen window whose content should be shown at flip time. "
	"\"when\" is the requested stimulus onset time, a value of zero or no argument asks for flip "
	"at next possible vertical retrace.\n"
	"For the meaning and explanation of all other parameters, see the help for 'Screen Flip?'.\n\n"
	"This command doesn't return anything, you'll need to use the finalizer commands mentioned below "
	"to collect information about the final result and timing of the flip operation.\n\n"
	"The difference between Screen('AsyncFlipBegin',...); and the more commonly used Screen('Flip', ...); "
	"is that Screen('Flip') operates synchronously: Execution of your code is paused until the flip operation "
	"has finished, ie. at least until the requested onset deadline 'when' has passed.\n\n"
	"Screen('AsyncFlipBegin') will prepare everything for a flip at the requested time 'when' - or "
	"at next vertical retrace if 'when' is omitted - but then immediately return control to your "
	"code. Your code can continue to execute and do other things, e.g., schedule flips for other onscreen "
	"windows, perform keyboard or mouse queries, etc.\n\n"
	"You can check the progress state of asynchronous flips or wait for them to finish and collect timing "	
	"information for the finalized flip by use of the commands Screen('AsyncFlipCheckEnd') and Screen('AsyncFlipEnd');\n\n"
	"In general you should avoid using asynchronous flips and instead use conventional 'Flip' unless you have a "
	"good reason to do otherwise, because async flips come with a couple of strings attached:\n\n"
	"- They are not portable to MS-Windows: Code written with async flip will only \n"
	"  work on Apple OS/X and GNU/Linux. There are no plans to ever support this\n"
	"  feature on MS-Windows, we don't want to beat that shaky beast to death...\n\n"
	"- You are restricted in what you can do with Screen() or OpenGL while async\n"
	"  flips are in progress: You can not do anything with textures or offscreen\n"
	"  windows while any onscreen window is in async flip state. You can only\n"
	"  access onscreen windows which are not participating in an async flip\n"
	"  operation. Even the restricted set of allowed Screen/OpenGL commands should\n"
	"  be avoided if possible, because some graphics hardware and drivers may not be\n"
	"  able to handle such concurrent graphics operations without degraded stimulus\n"
	"  onset timing accuracy, ie. you may experience more missed stimulus deadlines\n"
	"  and timing glitches -- or inconsistent behaviour accross different computers\n"
	"  and graphics cards or operating system releases. In the end it allows you to\n"
	"  do non-Screen related things like sound, I/O, keyboard checks...\n\n"
	"- Parallel processing of flips puts additional burden and load onto your CPU,\n"
	"  GPU and operating system, so it incurs additional overhead and may degrade\n"
	"  absolute drawing performance and cause more timing issues and glitches if\n"
	"  your system is not reliably able to handle the concurrent load.\n\n"
	"- Code with async flips - as any piece of parallel code - is harder to\n"
	"  get right and more challenging to debug for you.\n\n"
	"- Using a non-zero \"multiflip\" argument is not allowed.\n\n"
	"- Asynchronous updates of gamma tables may not work reliably.\n\n"
	"- Use of the 'UserspaceBufferDrawingPrepare' hook-chain of the imaging\n"
	"  pipeline is not allowed.\n"
	"\n\n"
	"Our general stance is that most code can be written efficiently without need for async flips, so this feature is here "
	"for the few cases where this is not the case and the benefits outweight the costs.";

	static char useString2[] = "[VBLTimestamp StimulusOnsetTime FlipTimestamp Missed Beampos] = Screen('AsyncFlipEnd', windowPtr);";
	static char synopsisString2[] = 
	"Wait for completion of a previously scheduled asynchronous flip operation (see Screen AsyncFlipBegin? for help).\n"
	"This command will wait for completion on onscreen window \"windowPtr\", then return the result of the operation, "
	"ie. all the stimulus onset timestamps and other diagnostic information. See help for Screen Flip? for explanation "
	"of the returned info.\n"
	"Screen('AsyncFlipCheckEnd') provides a non-blocking, polling version of this command -- one that doesn't pause if "
	"the referenced operation hasn't completed yet.";

	static char useString3[] = "[VBLTimestamp StimulusOnsetTime FlipTimestamp Missed Beampos] = Screen('AsyncFlipCheckEnd', windowPtr);";
	static char synopsisString3[] = 
	"Check if a previously scheduled asynchronous flip operation has completed (see Screen AsyncFlipBegin? for help).\n"
	"This command will check for completion on onscreen window \"windowPtr\", then return the result of the operation, "
	"ie. all the stimulus onset timestamps and other diagnostic information. See help for Screen Flip? for explanation "
	"of the returned info. If the operation hasn't completed yet, it will return a 'VBLTimestamp' of zero to signal that "
	"condition and you'll have to retry later.\n"
	"Screen('AsyncFlipEnd') provides a blocking version of this command -- one that pauses until the operation completes if "
	"the referenced operation hasn't completed yet.";

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
	boolean flipstate;

#if PSYCH_SYSTEM == PSYCH_OSX || PSYCH_SYSTEM == PSYCH_LINUX

	// Change our "personality" depending on the name with which we were called:
	if (PsychMatch(PsychGetFunctionName(), "AsyncFlipBegin")) {
		// Async flip invocation:
		opmode = 1;

		// Push usage string and/or give online help:
		PsychPushHelp(useString1, synopsisString1, seeAlsoString);
	}
	else if (PsychMatch(PsychGetFunctionName(), "AsyncFlipEnd")) {
		// Finalize async flip operation:
		opmode = 2;

		// Push usage string and/or give online help:
		PsychPushHelp(useString2, synopsisString2, seeAlsoString);
	}
	else if (PsychMatch(PsychGetFunctionName(), "AsyncFlipCheckEnd")) {
		// Poll for finalization of async flip operation:
		opmode = 3;

		// Push usage string and/or give online help:
		PsychPushHelp(useString3, synopsisString3, seeAlsoString);
	}
	else {
		// Standard synchronous flip:
		opmode = 0;

		// Push usage string and/or give online help:
		PsychPushHelp(useString0, synopsisString0, seeAlsoString);
	};
#else
	// Windows: No async flip support:
	// Standard synchronous flip:
	opmode = 0;

	// Push usage string and/or give online help:
	PsychPushHelp(useString0, synopsisString0, seeAlsoString);
#endif
	
	// Give online help, if requested:
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
		
		// Current multiflip > 0 implementation is not thread-safe, so we don't support this in async flip mode:
		if ((multiflip != 0) && (opmode != 0))  PsychErrorExitMsg(PsychError_user, "Using a non-zero 'multiflip' flag while starting an asynchronous flip! Sorry, this is currently not possible.\n");
		// Imaging pipeline hooks are not thread-safe: This one is the only one in PsychPostFlipOperations(); so we
		// disallow it. The many hooks in PsychPreflipOperations() are safe because we perform the preflip-ops in the
		// setup of async flip - before we enter the async helper thread:
		if (PsychIsHookChainOperational(windowRecord, kPsychUserspaceBufferDrawingPrepare) && (opmode != 0)) PsychErrorExitMsg(PsychError_user, "Trying to use the 'UserspaceBufferDrawingPrepare' imaging hook-chain while starting an asynchronous flip! Sorry, this is currently not possible.\n");

		// Query optional flipwhen argument: -1 == Don't sync to vertical retrace --> Only useful for debugging!
		// Use this only if you "really know what you're doing(TM)!"
		// 0 (default value) == Flip at next vertical retrace and sync to VBL. This is the old PTB behaviour as of PTB 1.0.42.
		// flipwhen>0 == Sleep/Wait until system time "flipwhen" and then flip at the next VBL after time "flipwhen".
		flipwhen=0;
		PsychCopyInDoubleArg(2,FALSE,&flipwhen);
		if(flipwhen < 0) {
			PsychErrorExitMsg(PsychError_user, "Only 'when' values greater or equal to 0 are supported");
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
			PsychErrorExitMsg(PsychError_user, "Screen('Flip'); or Screen('AsyncFlipBegin'); called while a scheduled flip operation is still in progress! Forbidden! Check your code!!");			
		}

		flipRequest->opmode			= opmode;
		flipRequest->dont_clear		= dont_clear;
		flipRequest->flipwhen		= flipwhen;
		flipRequest->multiflip		= multiflip;
		flipRequest->vbl_synclevel	= vbl_synclevel;		

		// Ok, the struct is filled with spec for a synchronous or asynchronous flip...
		
		// Store current preflip GPU graphics surface addresses, if supported:
		PsychStoreGPUSurfaceAddresses(windowRecord);
	}
	else {
		// opmode == 2 or 3
		// 'FlipAsyncEnd' or 'FlipAsyncPoll': Just make sure the flip info record is actually allocated:
		if (NULL == windowRecord->flipInfo) {
			// Flip end called without any flipInfo struct allocated?!? A flip op wasn't ever triggered!
			PsychErrorExitMsg(PsychError_user, "Screen('AsyncFlipEnd'); or Screen('AsyncFlipCheckEnd'); called without calling Screen('AsyncFlipBegin') before to actually start an async Flip! Forbidden! Check your code!!");
		}

		flipRequest = windowRecord->flipInfo;
		if (flipRequest->asyncstate == 0) {
			// No started, executing or finalized async flip in progress! No async flip operation triggered
			// which we could finalize!
			PsychErrorExitMsg(PsychError_user, "Screen('AsyncFlipEnd'); or Screen('AsyncFlipCheckEnd'); called without calling Screen('AsyncFlipBegin') before to actually start an async Flip! Forbidden! Check your code!!");
		}

		// Ok, there's an async flip going on and we have a handle to it...
		flipRequest->opmode	= opmode;
	}

	// Execute the flip request with the indirect routine: In opmode == 0 (Synchronous flip)
	// this will block until flip is done, then return with the results. In async mode it
	// will dispatch the flip to the helper thread, then return immediately. In async end
	// or async poll mode, it will either block until async op finished, or return after
	// polling with a FALSE result, telling that not yet finished:
	flipstate = PsychFlipWindowBuffersIndirect(windowRecord);
	
	// Only have return args in synchronous mode or in return path from end/successfull poll of async flip:
	if (opmode != 1) {
		// Async flip is either zero in synchronous mode, or its 2 if an async flip
		// successfully finished:
		if ((flipRequest->asyncstate!=0) && (flipRequest->asyncstate!=2) && (flipstate)) {
			printf("PTB-ERROR: flipRequest->asyncState has impossible value %i at end of flipop! This is a PTB DESIGN BUG!", flipRequest->asyncstate);
			PsychErrorExitMsg(PsychError_internal, "flipRequest->asyncState has impossible value at end of flipop! This is a PTB DESIGN BUG!");
		}
		
		// Reset it to zero, ie. ready for new adventures ;-)
		if (flipstate) flipRequest->asyncstate=0;
		
		// Return return arguments from flip: We return a zero vbl_timestamp in case a poll for flip completion failed.
		// That indicates that flip not yet finished and all other return values are invalid:
		vbl_timestamp	= (flipstate) ? flipRequest->vbl_timestamp : 0.0;
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

PsychError SCREENWaitUntilFlipCertain(void) 
{
	// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
	static char useString[] = "[VBLTimestamp StimulusOnsetTime swapCertainTime] = Screen('WaitUntilFlipCertain', windowPtr);";
	static char synopsisString[] =
	"Wait until it is certain that a previously initiated Screen('Flip',..) or Screen('AsyncFlipBegin',...); "
	"operation for onscreen window 'windowPtr' will happen or has happened already, then return timestamps "
	"of when flip operation has happened or when the flip operation will happen.\n"
	"This function only works on Linux and MacOS/X and only with recent AMD/ATI hardware of the "
	"Radeon X1000/HD2000/... series or later (or the equivalent FireGL cards). On MacOS/X, the "
	"PsychtoolboxKernelDriver needs to be loaded for this to work (see help PsychtoolboxKernelDriver).\n"
	"The method tries to \"read the mind\" of your graphics card to find out if the card will swap the "
	"display buffers at the next vertical retrace to find out if the swap is certain, ie., not possibly "
	"delayed by graphics card overload, other overload conditions or system timing jitter/delays.\n"
	"Ideally it will allow your script to know when stimulus onset will happen a few milliseconds before "
	"stimulus onset. This allows, e.g., to initiate some other operations that need to be tightly and "
	"reliably synchronized to stimulus onset and that have some small startup latency, so they need some "
	"headstart. Examples would be sound output or output of some slow trigger signal which takes multiple "
	"milliseconds to take action.\n"
	"This feature only works in fullscreen mode and is considered experimental - It may or may not work "
	"on your setup. Good luck!\n"
	"Returns a high-precision estimate of the system time (in seconds) when the actual flip will- or has happened "
	"in the return argument 'VBLTimestamp'. An estimate of Stimulus-onset time is returned in 'StimulusOnsetTime'. "
	"'swapCertainTime' is the system time when certainty of bufferswap was detected. ";
	static char seeAlsoString[] = "";
	
	PsychWindowRecordType *windowRecord;
	double timestamp, stimulusOnset;
	int	   beamposition;
	double vbl_startline, vbl_endline, vbl_lines_elapsed, onset_lines_togo;
	double vbl_time_elapsed, onset_time_togo, currentrefreshestimate;
	bool   swappending;
	long   scw, sch;
	
	// Push usage string and/or give online help:
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	
	// Give online help, if requested:
	if(PsychIsGiveHelp()) {PsychGiveHelp();return(PsychError_none);};

	PsychErrorExit(PsychCapNumInputArgs(1));		// The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1));	// The required number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(3));		// The maximum number of outputs

#if PSYCH_SYSTEM == PSYCH_OSX || PSYCH_SYSTEM == PSYCH_LINUX
	
	// Get the window record from the window record argument and get info from the window record
	PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);

	// Child protection:
	if (!PsychIsOnscreenWindow(windowRecord)) PsychErrorExitMsg(PsychError_user, "WaitUntilFlipCertain called on something else than an onscreen window.");
	if(windowRecord->windowType!=kPsychDoubleBufferOnscreen) PsychErrorExitMsg(PsychError_user, "WaitUntilFlipCertain called on window without backbuffers.");
	
	// Just need to check if GPU low-level access is supported:
	if (!PsychOSIsKernelDriverAvailable(windowRecord->screenNumber)) {
		PsychErrorExitMsg(PsychError_user, "WaitUntilFlipCertain needs the PsychtoolboxKernelDriver to be loaded on supported hardware!");
	}

    // Retrieve estimate of monitor refresh interval:
    if (windowRecord->VideoRefreshInterval > 0) {
        currentrefreshestimate = windowRecord->VideoRefreshInterval;
    }
    else {
        currentrefreshestimate=0;
        // We abort - This is too unsafe...
        PsychErrorExitMsg(PsychError_user,"Estimate of monitor refresh interval is INVALID -> Aborting!");
    }

    if (windowRecord->VBL_Endline == -1) {
        PsychErrorExitMsg(PsychError_user,"Estimate of VBL endline is INVALID -> Aborting!");
	}

	// Wait for bufferswap completion or bufferswap certain:
	swappending = PsychWaitForBufferswapPendingOrFinished(windowRecord, &timestamp, &beamposition);

	// Copy out optional timestamp of bufferswap detection:
	PsychCopyOutDoubleArg(3, kPsychArgOptional, timestamp);

	// Compute timestamps:
	PsychGetScreenSize(windowRecord->screenNumber, &scw, &sch);
    vbl_startline = (int) sch;
	vbl_endline = windowRecord->VBL_Endline;

	if (swappending) {
		// Swap is pending:
		onset_lines_togo = vbl_endline - beamposition + 1;
		vbl_lines_elapsed = vbl_startline - beamposition + 1; 
		
		// From the elapsed number we calculate the remaining time till swap et al.:
		vbl_time_elapsed = vbl_lines_elapsed / vbl_endline * currentrefreshestimate; 
		onset_time_togo = onset_lines_togo / vbl_endline * currentrefreshestimate;

		// Add to basetime:
		stimulusOnset = timestamp + onset_time_togo;
		timestamp = timestamp + vbl_time_elapsed;
		
		// printf("PENDING\n");
	}
	else {
		// Swap happened already: Do same calculations as in PsychFlipWindowBuffers():
		
		if (beamposition >= vbl_startline) {
			vbl_lines_elapsed = beamposition - vbl_startline;
			onset_lines_togo = vbl_endline - beamposition + 1;
		}
		else {
			vbl_lines_elapsed = vbl_endline - vbl_startline + 1 + beamposition;
			onset_lines_togo = -1.0 * beamposition;
		}
		
		// From the elapsed number we calculate the elapsed time since VBL start:
		vbl_time_elapsed = vbl_lines_elapsed / vbl_endline * currentrefreshestimate; 
		onset_time_togo = onset_lines_togo / vbl_endline * currentrefreshestimate;
		
		// Compute of stimulus-onset, aka time when retrace is finished:
		stimulusOnset = timestamp + onset_time_togo;
		// Now we correct our time_at_vbl by this correction value:
		timestamp = timestamp - vbl_time_elapsed;
	}
	
	// Copy out optional vbl timestamp of bufferswap:
	PsychCopyOutDoubleArg(1, kPsychArgOptional, timestamp);

	// Copy out optional timestamp of stimulus onset:
	PsychCopyOutDoubleArg(2, kPsychArgOptional, stimulusOnset);
#else
	PsychErrorExitMsg(PsychError_user, "Screen('WaitUntilFlipCertain') is not supported on Microsoft Windows.");
#endif

	// Done.
	return(PsychError_none);	
}
