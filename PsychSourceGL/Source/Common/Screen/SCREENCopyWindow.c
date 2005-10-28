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

 
	TO DO:

		Add support for 8-bit pixels
		Add support for 16-bit pixels

		Accept the copy mode argument.  We use alpha blending so we don't really need it, 
		but we should detect if someone tries to use one.  

		Add another field to the window record for specifying the texture target screen instread
		of using the screen number for that.   It doesn't really work to share the same field
		to mean both the screen on which an onscreen window appears and the destination to which 
		screen to which its copied.  'WindowScreenNumber' should be changed so that it returns
		two values, the display number which for offscreen windows will alwyas be NaN or -1,
		and the accellerated target window.  

*/




#include "Screen.h"


static char useString[] =  "Screen('CopyWindow',srcWindowPtr,dstWindowPtr,[srcRect],[dstRect],[copyMode])";
static char synopsisString[] = "Copy images, very quickly, between two windows (on- or off- screen).";

static char seeAlsoString[] = "PutImage, GetImage";
	

PsychError SCREENCopyWindow(void) 
{
	PsychRectType			sourceRect, targetRect, targetRectInverted;
	PsychWindowRecordType	*sourceWin, *targetWin;
	GLdouble				sourceVertex[2], targetVertex[2]; 
	double  t1;
	double					sourceRectWidth, sourceRectHeight;
        
    
    
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
	PsychCopyRect(targetRect, targetWin->rect);
	PsychCopyInRectArg(4, FALSE, targetRect);

	//Check that the windows agree in depth.  They don't have to agree in format because we convert to the correct format for the target when we 
	//create the texture.
	if(sourceWin->depth != targetWin->depth)
		PsychErrorExitMsg(PsychError_user, "Source and target windows must have the same bit depth");
            
	//We need to unbind the source window texture from any previous target contexts if it is bound.  There can be only one binding at a time.
	//We could augment the Psychtoolbox to support shared contexts and multiple bindings.
	PsychRetargetWindowToWindow(sourceWin, targetWin);
		
		
	//each of these next three commands makes changes only when neccessary, they can be called generously 
	//when there is a possibility that any are necessary.
	  
	//PsychGetPrecisionTimerSeconds(&t1);
	PsychUpdateShadow(sourceWin);			//passive if already up to date.
	PsychBindShadowAsTexture(sourceWin);
	//PsychGetPrecisionTimerSeconds(&t2);
	//mexPrintf("texture checking copywindow took %f seconds\n", t2-t1);

	//PsychGetPrecisionTimerSeconds(&t1);
	PsychGetPrecisionTimerSeconds(&t1);
	PsychSetGLContext(targetWin);
	
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, sourceWin->glTexture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	sourceRectWidth= PsychGetWidthFromRect(sourceWin->rect);
	sourceRectHeight= PsychGetHeightFromRect(sourceWin->rect);
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, 0, 0, (GLsizei)sourceRectWidth, (GLsizei)sourceRectHeight, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, sourceWin->textureMemory);
	
	PsychInvertRectY(targetRectInverted, targetRect, targetWin->rect);
	//PsychGetPrecisionTimerSeconds(&t1);
    glBegin(GL_QUADS);
		glTexCoord2dv(PsychExtractQuadVertexFromRect(sourceRect, 0, sourceVertex));
		glVertex2dv(PsychExtractQuadVertexFromRect(sourceRect, 3, targetVertex));
		
		glTexCoord2dv(PsychExtractQuadVertexFromRect(sourceRect, 1, sourceVertex));
		glVertex2dv(PsychExtractQuadVertexFromRect(sourceRect, 2, targetVertex));

		glTexCoord2dv(PsychExtractQuadVertexFromRect(sourceRect, 2, sourceVertex));
		glVertex2dv(PsychExtractQuadVertexFromRect(sourceRect, 1, targetVertex));

		glTexCoord2dv(PsychExtractQuadVertexFromRect(sourceRect, 3, sourceVertex));
		glVertex2dv(PsychExtractQuadVertexFromRect(sourceRect, 0, targetVertex));
    glEnd();
    glFinish();
	//PsychGetPrecisionTimerSeconds(&t2);
	//mexPrintf("copywindow took %f seconds\n", t2-t1);
	PsychUnbindTextureOfShadow(sourceWin);


	return(PsychError_none);
}


