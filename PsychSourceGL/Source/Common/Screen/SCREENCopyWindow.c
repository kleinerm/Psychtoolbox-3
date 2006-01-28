/*
	PsychToolbox2/Source/Common/Screen/SCREENCopyWindow.c		
  
	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
		
		So far only OS X.  For now other platforms have different versions of this file.  
    
	HISTORY:
	
		02/19/03  	awi		Created.
		03/11/04	awi		Modified for textures
		 1/11/05	awi		Cosmetic
		 1/14/05	awi		added glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE) at mk's suggestion;
		 1/25/05	awi		Relocated glTexEnvf below glBindTexture.  Fix provided by mk.  
                 1/22/05        mk              Completely rewritten for the new OffscreenWindow implementation.
 
	TO DO:

		Add support for 8-bit pixels
		Add support for 16-bit pixels

		Accept the copy mode argument.  We use alpha blending so we don't really need it, 
		but we should detect if someone tries to use one.  

*/




#include "Screen.h"

static char useString[] =  "Screen('CopyWindow',srcWindowPtr,dstWindowPtr,[srcRect],[dstRect],[copyMode])";
static char synopsisString[] =  "Copy images, quickly, between two windows (on- or off- screen). "
                                "srcRect and dstRect are set to the size of srcWindowPtr and dstWindowPtr "
                                "by default. [copyMode] is accepted as input but currently ignored. "
                                "CopyWindow is mostly here for compatibility to OS-9 PTB. If you want to "
                                "copy images really quickly into an onscreen window, then use the 'MakeTexture' "
                                "and 'DrawTexture' commands. They also allow for rotated drawing and advanced "
                                "blending operations.";

static char seeAlsoString[] = "PutImage, GetImage, OpenOffscreenWindow, MakeTexture, DrawTexture";

PsychError SCREENCopyWindow(void) 
{
	PsychRectType		sourceRect, targetRect, targetRectInverted;
	PsychWindowRecordType	*sourceWin, *targetWin;
	GLdouble		sourceVertex[2], targetVertex[2]; 
	double  t1;
	double			sourceRectWidth, sourceRectHeight;
        
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

	//cap the number of inputs
	PsychErrorExit(PsychCapNumInputArgs(5));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs
        
	//get parameters for the source window:
	PsychAllocInWindowRecordArg(1, TRUE, &sourceWin);
	PsychCopyRect(sourceRect, sourceWin->rect);
	PsychCopyInRectArg(3, FALSE, sourceRect);

	//get paramters for the target window:
	PsychAllocInWindowRecordArg(2, TRUE, &targetWin);
        // By default, the targetRect is equal to the sourceRect, but centered in
        // the target window.
	PsychCopyRect(targetRect, targetWin->rect);
        PsychCenterRect(sourceRect, targetRect, targetRect);
	PsychCopyInRectArg(4, FALSE, targetRect);

	// We have four possible combinations for copy ops:
        // Onscreen -> Onscreen
        // Onscreen -> Texture
        // Texture  -> Texture
        // Texture  -> Onscreen
        
        // Texture -> something copy?
        if (sourceWin->windowType == kPsychTexture) {
            // Bind targetWin (texture or onscreen windows framebuffer) as
            // drawing target and just blit texture into it:
            PsychSetGLContext(sourceWin);

            // We use filterMode == 1 aka Bilinear filtering, so we get nice texture copies
            // if size of sourceRect and targetRect don't match and some scaling is needed.
            // We maybe could map the copyMode argument into some filterMode settings, but
            // i don't know the spec of copyMode, so ...
            PsychBlitTextureToDisplay(sourceWin, targetWin, sourceRect, targetRect, 0, 1, 1);
            // That's it.

            // Flush drawing commands and wait for render-completion in single-buffer mode:
            PsychFlushGL(targetWin);
        }
        
        // Onscreen to texture copy?
        if (PsychIsOnscreenWindow(sourceWin) && PsychIsOffscreenWindow(targetWin)) {
            // Update selected textures content:
            PsychSetGLContext(targetWin);

            // Looks weird but we need the framebuffer of sourceWin:
            PsychSetDrawingTarget(sourceWin);
            glBindTexture(PsychGetTextureTarget(targetWin), targetWin->textureNumber);

            // Zoom factor if rectangle sizes don't match:
            glPixelZoom(PsychGetWidthFromRect(targetRect) / PsychGetWidthFromRect(sourceRect), PsychGetHeightFromRect(targetRect) / PsychGetHeightFromRect(sourceRect));

            // Copy into texture:
            glCopyTexSubImage2D(PsychGetTextureTarget(targetWin), 0, targetRect[kPsychLeft], targetRect[kPsychTop], sourceRect[kPsychLeft], sourceRect[kPsychBottom],
                                (int) PsychGetWidthFromRect(targetRect), (int) PsychGetHeightFromRect(targetRect));

            // Unbind texture object:
            glBindTexture(PsychGetTextureTarget(targetWin), 0);

            // That's it.
            glPixelZoom(1,1);
        }
        
        // Onscreen to Onscreen copy?
        if (PsychIsOnscreenWindow(sourceWin) && PsychIsOnscreenWindow(targetWin)) {
            // Enable OpenGL context of source window:
            PsychSetGLContext(sourceWin);
            // Set target windows framebuffer as drawing target:
            PsychSetDrawingTarget(targetWin);
            // Start position for pixel write is:
            glRasterPos2i((int) targetRect[kPsychLeft], (int) targetRect[kPsychTop]);
            // Zoom factor if rectangle sizes don't match:
            glPixelZoom(PsychGetWidthFromRect(targetRect) / PsychGetWidthFromRect(sourceRect), PsychGetHeightFromRect(targetRect) / PsychGetHeightFromRect(sourceRect));
            // Perform pixel copy operation:
            glCopyPixels(sourceRect[kPsychLeft], sourceRect[kPsychBottom], (int) PsychGetWidthFromRect(sourceRect), (int) PsychGetHeightFromRect(sourceRect), GL_COLOR);
            // That's it.
            glPixelZoom(1,1);
            // Flush drawing commands and wait for render-completion in single-buffer mode:
            PsychFlushGL(sourceWin);
        }
        
        // Done.
        return(PsychError_none);
}


