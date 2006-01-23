/*
	SCREENWaitBlanking.c		
  
	AUTHORS:

		mario.kleiner at tuebingen.mpg.de 		mk 
  
	PLATFORMS:	
	
		All.
    

	HISTORY:

		01/23/06	mk		Created.

	DESCRIPTION:
  
                Waits for beginning of vertical retrace. Blocks Matlabs or PTB's execution until then.
                Screen('WaitBlanking') is here mostly for backwards-compatibility with old OS-9 and WinPTB.
                It is not the recommended way of doing things for new code!
 
	TO DO:  

*/


#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "framesSinceLastWait = Screen('WaitBlanking', windowPtr [, waitFrames]);";
static char synopsisString[] = 
	"Wait for specified number of monitor refresh intervals, stopping PTB's "
        "execution until then. Select waitFrames=1 (or omit it since that's the default) "
        "to wait for the beginning of the next frame. "
        "\"windowPtr\" is the pointer to the onscreen window for which we should wait for. "
        "framesSinceLastWait contains the number of video refresh intervals from the last "
        "time Screen('WaitBlanking') or Screen('Flip') was executed until return from "
        "this call to WaitBlanking. Please note that this function is only provided to keep old code "
        "from OS-9 PTB running. Use the Screen('Flip') command for all new code as this "
        "allows for much higher accuracy and reliability of stimulus timing and enables "
        "a huge number of new and very useful features! ";

static char seeAlsoString[] = "OpenWindow Flip";

PsychError SCREENWaitBlanking(void) 
{
	PsychWindowRecordType *windowRecord;
   int waitFrames, framesWaited;
   double tvbl, ifi;
	long screenwidth, screenheight;
   int vbl_startline, beampos, oldbeampos;
   CGDirectDisplayID	cgDisplayID;
   GLint read_buffer, draw_buffer;


	// All subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	PsychErrorExit(PsychCapNumInputArgs(2));     //The maximum number of inputs
        PsychErrorExit(PsychRequireNumInputArgs(1)); //The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));  //The maximum number of outputs
        
	// Get the window record from the window record argument and get info from the window record
	PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
        
        if(!PsychIsOnscreenWindow(windowRecord))
            PsychErrorExitMsg(PsychError_user, "Tried to call 'WaitBlanking' on something else than an onscreen window!");
    
        // Get the number of frames to wait:
        waitFrames = 0;
        PsychCopyInIntegerArg(2, FALSE, &waitFrames);
        waitFrames = (waitFrames < 1) ? 1 : waitFrames;

	// Switch to associated GL-Context of onscreen window:
        PsychSetGLContext(windowRecord);
        
        // Enable this windowRecords framebuffer as current drawingtarget:
        // Don't sure if we need it here, but it can't hurt ;)
        PsychSetDrawingTarget(windowRecord);
    
        // Retrieve display handle for beamposition queries:
        PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, windowRecord->screenNumber);
        
        // Retrieve final vbl_startline, aka physical height of the display in pixels:
        PsychGetScreenSize(windowRecord->screenNumber, &screenwidth, &screenheight);
        vbl_startline = (int) screenheight;
        
        // Get duration of a monitor refresh interval:
        if ((ifi = windowRecord->VideoRefreshInterval)<=0) {
            if (PsychGetNominalFramerate(windowRecord->screenNumber) > 0) {
                // Valid nominal framerate returned by OS: Calculate nominal IFI from it.
                ifi = 1.0 / ((double) PsychGetNominalFramerate(windowRecord->screenNumber));        
            }
            else {
                ifi = 1.0 / 60.0;
            }
        }
        
        // Check if beamposition queries are supported:
        if (TRUE && -1 != CGDisplayBeamPosition(cgDisplayID) && windowRecord->VBL_Endline >= 0) {
            // Beamposition queries supported. We can wait for VBL without bufferswap-tricks:
            while(waitFrames > 0) {
                // Enough time for a sleep?
                beampos = CGDisplayBeamPosition(cgDisplayID);
                while (( ((float)(vbl_startline - beampos)) / (float) vbl_startline * ifi) > 0.002) {
                    // At least 2 milliseconds left until retrace. We sleep for 1 millisecond.
                    PsychWaitIntervalSeconds(0.001);
                    beampos = CGDisplayBeamPosition(cgDisplayID);
                }
                
                // Less than 2 ms away from retrace. Busy-Wait:
                while (CGDisplayBeamPosition(cgDisplayID) < vbl_startline);
                
                // Retrace! Take system timestamp of VBL onset:
                PsychGetAdjustedPrecisionTimerSeconds(&tvbl);

                // Wait for end of retrace:
                oldbeampos=-1;
                while ((beampos=CGDisplayBeamPosition(cgDisplayID)) >= oldbeampos) { oldbeampos = beampos; };
                
                // Done with this refresh interval...
                // Decrement remaining number of frames to wait:
                waitFrames--;
            }
        }
        else {            
            // Beamposition queries unsupported. We use the doublebuffer swap method of syncing.
            // This method is used on Microsoft-Windows where we don't have a function for beamposition queries.
            // It shouldn't be used on OS-X.
            if (windowRecord->windowType != kPsychDoubleBufferOnscreen) {
                PsychErrorExitMsg(PsychError_internal, "WaitBlanking tried to perform swap-waiting on single buffered window!");
            }
            
            // Setup buffers for copy op:
            glGetIntegerv(GL_READ_BUFFER, &read_buffer);
            glGetIntegerv(GL_DRAW_BUFFER, &draw_buffer);
            glReadBuffer(GL_FRONT);
            glDrawBuffer(GL_BACK);
            // Reset viewport to full-screen default:
            glViewport(0, 0, screenwidth, screenheight);                
            // Reset color buffer writemask to "All enabled":
            glColorMask(TRUE, TRUE, TRUE, TRUE);
            glDisable(GL_BLEND);
            
            while(waitFrames > 0) {
                // Copy current content of front buffer into backbuffer:
                glRasterPos2i(0, 0);
                glCopyPixels(0, 0, screenwidth, screenheight, GL_COLOR);            

                // Ok, front- and backbuffer are now identical, so as bufferswap
                // will be a visual no-op.
                
                // Enable beamsyncing of bufferswaps to VBL:
                PsychOSSetVBLSyncLevel(windowRecord, 1);                
                // Swap buffer in sync with retrace:
                PsychOSFlipWindowBuffers(windowRecord);

                // Wait for swap-completion:
                glColor4f(0,0,0,0);
                glBegin(GL_POINTS);
                glVertex2i(10,10);
                glEnd();
                glFinish();
                
                // Take system timestamp:
                PsychGetAdjustedPrecisionTimerSeconds(&tvbl);

                // Enable beamsyncing of bufferswaps to VBL:
                PsychOSSetVBLSyncLevel(windowRecord, 0);                
                // Swap buffer in sync with retrace:
                PsychOSFlipWindowBuffers(windowRecord);
                
                // Decrement remaining number of frames to wait:
                waitFrames--;
            }
            
            // Enable beamsyncing of bufferswaps to VBL:
            PsychOSSetVBLSyncLevel(windowRecord, 1);                

            glEnable(GL_BLEND);
            // Restore assignment of read- writebuffers:
            glReadBuffer(read_buffer);
            glDrawBuffer(draw_buffer);        
        }
                
        // Compute number of frames waited:
        framesWaited = (int) (((tvbl - windowRecord->time_at_last_vbl) / ifi) + 0.5f);
        
        // Update timestamp:
        windowRecord->time_at_last_vbl = tvbl;
        
        // Return optional number of frames waited:
        PsychCopyOutDoubleArg(1, FALSE, (double) framesWaited);
        
        // Done.
        return(PsychError_none);
}
