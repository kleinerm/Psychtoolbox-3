/*
  SCREENDrawingFinished.cpp		
  
  AUTHORS:
    mk          mario.kleiner at tuebingen.mpg.de
 
  PLATFORMS:	All
    
  HISTORY:
  04/10/05  mk		Created.  
 
 
  DESCRIPTION:
    This command can be put into a Matlab script after the last drawing command and before the next
    Flip - command as a hint to PTB that no further drawing commands will follow before next Flip.
    
    e.g.
 
    ...
    All stimulus drawing commands....
    ...
    SCREEN('DrawingFinished',...)
    ... All commands unrelated to drawing like checking the keyboard, mouse, whatever...
    SCREEN('Flip', ...)
 
    This command issues a glFlush() to OpenGL: Normally, OpenGL / MacOS-X queues OpenGL drawing commands in
    its internal DMA command buffer chain. A command buffer is "flushed" to the graphics hardware for execution
    of drawing commands only if either the buffer is full, or if some special commands are put into the buffer
    which need immediate execution, or as part of the CGLFlushDrawable - call that marks a frame as "finished".
    This queuing is implemented for performance reason: In normal VR/Game applications that draw a lot of complex
    primitives per frame, this "batching" of drawing commands guarantees that the graphics pipeline doesn't run empty
    -> less pipeline stalls -> GPU is kept busy -> maximum parallelism between GPU and CPU -> higher throughput
    -> higher overall rendering performance -> happy customers and video game players!
 
    One can force a flush of all filled / half-filled buffers by issuing glFinish() or glFlush(): This is
    generally not recommended by Apple (or any other vendor), because the number of buffers is limited and flushing
    a buffer takes some time (overhead). Therefore, if you intersperse too many glFlush()'es -> flushing nearly
    empty buffers, you can quickly run out of free buffers --> CPU has to stall, waiting for new buffers to become
    available! --> Bad rendering performance!
 
    Problem with PTB: Many (probably most) experiment scripts issue only very few OpenGL drawing commands per
    stimulus frame (e.g., only 1 DrawTexture command for image/movie presentation, only few other commands for
    stimuli generated "on-the-fly"). This way it can happen that only 1 command buffer is partially filled -> No
    automatic flushing before CGLFlushDrawable is called as part of Screen('Flip') -> Rendering only starts at
    Flip-Time -> If stimulus is complex enough, e.g., big texture to draw, this can lead to a deadline miss for
    the next VBL although the gfx-hardware is powerful enough, because drawing starts too late in the frame cycle!
 
    The DrawingFinished command issues a glFlush() to enforce a start of all drawing operations early in the trial-
    loop -> GPU has nearly a whole monitor refresh interval for drawing the stim, while the CPU can execute other
    Matlab commands in parallel, e.g., kbCheck, GetMouse, GetSecs, sound playback, experiment control code, ...
 
    Alternatively, a glFinish() can be executed for timing of drawing commands, although this stalls the CPU and
    should be avoided for experiments.
 
    A second function is preparation of a Flip ahead of time: If the Flip command is asked to not clear the backbuffer
    after a flip, but restore the content of the buffer for incremental drawing of stims, this is implemented by
    copying the backbuffer to an AUX buffer before the Flip and restoring the backbuffer from AUX buffer after the
    Flip. Copy-ops take time (1-3 msecs on a GeForceFX5200-Ultra), so we try to perform the pre-flip copy op already
    as part of DrawingFinished. If the Psychtoolbox imaging processing pipeline is enabled, this will run a full
    execution cycle of the pipeline as well.
 
    Some flags indicate to the Flip command that a DrawingFinished has been performed to avoid redundant copy ops and
    redundant calls to glFlush()...
 
  TO DO:
  
*/


#include "Screen.h"

static char useString[] = "[telapsed] = SCREEN('DrawingFinished', windowPtr [, dontclear][, sync]);";
static char synopsisString[] =
        "Tell Psychtoolbox that no further drawing commands will be issued to 'windowPtr' "
        "before the next Screen('Flip') or Screen('AsyncFlipBegin') command.\n\n"
	"This is a hint that allows to optimize drawing performance on some occasions.\n"
        "Don't issue this command multiple times between a Flip, it will degrade performance "
        "or even cause undefined stimulus display!\n\n"
        "You must provide the same value for 'dontclear' flag that you're going to pass to the "
        "following Flip command, if you pass such an optional flag to the Flip command.\n\n"
        "You can time the execution of all drawing commands between the most recent Flip "
        "and this command by setting the optional "
        "flag sync=1. In that case, telapsed is the elapsed time at drawing completion. "
        "Don't set the sync - Flag for real experiments, it will degrade performance!\n\n"
	"Some recent graphics cards provide a more fine-grained way to measure the time spent "
	"drawing and processing your stimulus. See the help of 'Screen GetWindowInfo?' "
	"about 'infoType' settings 5 and 6 on how to use that mechanism. That method "
	"also has the advantage of measuring precise without degrading overall performance.";

static char seeAlsoString[] = "Flip AsyncFlipBegin";

PsychError SCREENDrawingFinished(void) 
{
    PsychWindowRecordType *windowRecord;
    psych_bool syncflag=false;
    int dontclear=0;
    double t=0;
    psych_bool runPreFlipOps = TRUE;
    
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    PsychErrorExit(PsychCapNumInputArgs(3));     //The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(1)); //The required number of inputs	
    PsychErrorExit(PsychCapNumOutputArgs(1));    //The maximum number of outputs
    
    //get the window record from the window record argument and get info from the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);	
    //get optional dontclear-Flag: We default to 0 == "clear framebuffer on flip", if nothing is provided.
    PsychCopyInIntegerArg(2,FALSE, &dontclear);    
    //get Sync-Flag:
    PsychCopyInFlagArg(3, FALSE, &syncflag);
    
    // Make sure we don't execute on an onscreen window with pending async flip, as this would interfere
    // by touching the system backbuffer -> Impaired timing of the flip thread and undefined readback
    // of image data due to racing with the ops of the flipperthread on the same drawable.
    //
    // If this passes then PsychSetDrawingTarget() below will trigger additional validations to check
    // if execution of 'GetImage' is allowed under the current conditions for offscreen windows and
    // textures:
    if (PsychIsOnscreenWindow(windowRecord) && (windowRecord->flipInfo->asyncstate > 0)) runPreFlipOps = FALSE;

    // Perform preflip-operations: Backbuffer backups for the different dontclear-modes
    // and special compositing operations for specific stereo algorithms...
    if (runPreFlipOps) {
        PsychPreFlipOperations(windowRecord, dontclear);
    }
    else {
        PsychSetGLContext(windowRecord);
    }

    // At this point, the GL-context of windowRecord is active due to PsychPreFlipOp....
    if (syncflag) {
        // Synchronization between CPU (Matlab et al.) and GPU requested:
        glFinish();
        // Compute elapsed time between last VBL and finish'ing of drawing commands:
        if (windowRecord->time_at_last_vbl > 0) {
            PsychGetAdjustedPrecisionTimerSeconds(&t);
            t = t - windowRecord->time_at_last_vbl;
        }
    }
    else {
        // Just a pipeline-flush requested:
        glFlush();
    }
    
    // Tell Flip that final pipeline - flushing has been done already to avoid redundant flush'es:
    if (runPreFlipOps) windowRecord->PipelineFlushDone = true;
    
    // Return elapsed time for rendering since Flip:
    PsychCopyOutDoubleArg(1, FALSE, t);
    
    return(PsychError_none);
}
