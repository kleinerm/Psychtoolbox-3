/*
	SCREENSelectStereoDrawBuffer.c		
  
	AUTHORS:

		mario.kleiner at tuebingen.mpg.de 		mk 
  
	PLATFORMS:	
	
		OS X only for now.
    

	HISTORY:

		04/03/05	mk		Created.
		10/30/05        mk              Synopsis string changed.

	DESCRIPTION:
  
		Selects the target buffer for drawing commands on a stereoscopic display:
                All drawing commands after this command will apply to the selected buffer.
  
	TO DO:  

*/


#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('SelectStereoDrawBuffer', windowPtr, bufferid);";
static char synopsisString[] = 
	"Select the target buffer for draw commands in stereo display windows. "
        "This function only applies to stereo mode. \"windowPtr\" is the pointer to the onscreen "
        "stereo window. \"bufferid\" is either == 0 for selecting the left-eye buffer, == 1 for "
        "selecting the right-eye buffer or == 2 for selecting both buffers. ";
static char seeAlsoString[] = "OpenWindow Flip";	 

PsychError SCREENSelectStereoDrawBuffer(void) 
{
	PsychWindowRecordType *windowRecord;
        int bufferid=2;
    
	//all subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	PsychErrorExit(PsychCapNumInputArgs(2));     //The maximum number of inputs
        PsychErrorExit(PsychRequireNumInputArgs(2)); //The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs
        
	//get the window record from the window record argument and get info from the window record
	PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
        
        if(!PsychIsOnscreenWindow(windowRecord))
            PsychErrorExitMsg(PsychError_user, "Tried to select stereo draw buffer on something else than a onscreen window");
            
	if(windowRecord->windowType!=kPsychDoubleBufferOnscreen || windowRecord->stereomode == 0) {
            // Trying to select the draw target buffer on a non-stereo window: We just reset it to monoscopic default.
            glDrawBuffer(GL_BACK);
            return(PsychError_none);
        }
            
        // Get the buffer id (0==left, 1==right):
        PsychCopyInIntegerArg(2, TRUE, &bufferid);
        if (bufferid<0 || bufferid>2)
            PsychErrorExitMsg(PsychError_user, "Invalid bufferid provided: Must be 0 for left-eye, 1 for right-eye buffer or 2 for both");

	// Switch to associated GL-Context:
        PsychSetGLContext(windowRecord);
        
        // OpenGL native stereo?
        if (windowRecord->stereomode==kPsychOpenGLStereo) {
            // OpenGL native stereo via separate back-buffers: Select target draw buffer:
            switch(bufferid) {
                case 0:
                    glDrawBuffer(GL_BACK_LEFT);
                    break;
                case 1:
                    glDrawBuffer(GL_BACK_RIGHT);
                    break;
                case 2:
                    glDrawBuffer(GL_BACK);
                    break;
            }
        }
        
        // Vertical compression stereo?
        if (windowRecord->stereomode==2) {
            // Switch between drawing into top- and bottom-half of the single framebuffer:
            int screenwidth=(int) PsychGetWidthFromRect(windowRecord->rect);
            int screenheight=(int) PsychGetHeightFromRect(windowRecord->rect);

            switch(bufferid) {
                case 0:
                    glViewport(0, screenheight/2, screenwidth, screenheight/2);
                    break;
                case 1:
                    glViewport(0, 0, screenwidth, screenheight/2);
                    break;
                case 2:
                    glViewport(0, 0, screenwidth, screenheight);
                    break;
            }
        }

        // "Free fusion" stereo?
        if (windowRecord->stereomode==3) {
            // Switch between drawing into left- and right-half of the single framebuffer:
            int screenwidth=(int) PsychGetWidthFromRect(windowRecord->rect);
            int screenheight=(int) PsychGetHeightFromRect(windowRecord->rect);
            
            switch(bufferid) {
                case 0:
                    glViewport(0, 0, screenwidth/2, screenheight);
                    break;
                case 1:
                    glViewport(screenwidth/2, 0, screenwidth/2, screenheight);
                    break;
                case 2:
                    glViewport(0, 0, screenwidth, screenheight);
                    break;
            }
        }
	return(PsychError_none);
}


	
	





