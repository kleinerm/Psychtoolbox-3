/*
  SCREENTestTextureTwo.c	
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	All
    

  HISTORY:
  3/3/03  awi		Created.    
 
  DESCRIPTION:
  
  Accept one pointer to an offscreen window and one to an  onscreen window.  "Texturize" the contents of the offscreen window and copy them to the onscreen window.

  This is version two of TestTexture.  Herin we experiment with all of the ingredients necessary to impliment fast copy windows:
	¥Client side texture storage
	¥glCopyTexImage2D in conjunction with the above.
	
  NOTES:
  
  Specifying a target: 
	  We must specify gl context when creating the texture.  The gl context should be the context of the destination window where the texture will be drawn.  In the OS 9 psychtoolbox all 
	  offscreen windows are associated with an onscreen window or screen.  Therefore, this requirement imposed by textures is not more restrictive than the requirements of 
	  the OS 9 Psychtoolbx.  (This is no coincidence, fast copies require matched formates between destination and source for both OpenGL and QuickDraw.)
	  
	  We should be able to do better in gl than with QuickDraw by sharing textures between GL contexts.  Contexts may only be shared between surfaces with the same pixelformat and 
	  the same sets of renders.  That means that multiple displays connected to the same video card are eligible.  
	  
	
  
  
  RESOURCES:
  Apple's TextureRange example.
  http://developer.apple.com/samplecode/Sample_Code/Graphics_3D/TextureRange.htm
  
  GL client storage extension man page:
  http://oss.sgi.com/projects/ogl-sample/registry/APPLE/client_storage.txt

*/


#include "Screen.h"


static char useString[] = "SCREEN('TestTexture', sourceWindowPntr, destWindowPntr, sourceWindowRect, destWindowRect);";
static char synopsisString[] = 
	"Test out textures for implementing  offscreen windows.";
static char seeAlsoString[] = "";


PsychError SCREENTestTextureTwo(void) 
{
#if PSYCH_SYSTEM == PSYCH_OSX

    PsychWindowRecordType 	*sourceWinRec, *destWinRec;
    unsigned int			memorySizeBytes;
    UInt32					*textureMemory;
    GLuint					myTexture;
	int						sourceWinWidth, sourceWinHeight;

    double					t1, t2;
    			
    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous or missing arguments
	PsychErrorExit(PsychCapNumInputArgs(2));   	
    PsychErrorExit(PsychRequireNumInputArgs(2)); 	

    
    //Get the window structure for the onscreen and offscreen windows.
	PsychAllocInWindowRecordArg(1, TRUE, &sourceWinRec);
	PsychAllocInWindowRecordArg(2, TRUE, &destWinRec);

        
	//Now allocate memory for our texture.   This is a still a hack so we assume 32 bit pixels which could be overgenerous.  
	sourceWinWidth=(int)PsychGetWidthFromRect(sourceWinRec->rect);
	sourceWinHeight=(int)PsychGetWidthFromRect(sourceWinRec->rect);
	memorySizeBytes=sizeof(GLuint) * sourceWinWidth * sourceWinHeight;
	textureMemory=(GLuint *)malloc(memorySizeBytes);
    if(!textureMemory)
		PsychErrorExitMsg(PsychError_internal, "Failed to allocate surface memory\n");
	
	//Now read the contents of the source window in to our memory with glReadPixels.
	//The pixel format in which we pack the pixels, specified using glPixelStorei() and glReadPixels()
	//arguments should agree with the format used by glTexImage2D when we turn it into a texture below.
	//glTextImage2D is constrained by the requirement that it math the video memory pixel format,
	//therefore there is really now choice here either.  
	PsychSetGLContext(sourceWinRec);
	glReadBuffer(GL_FRONT);
	glPixelStorei(GL_PACK_ALIGNMENT, (GLint)sizeof(GLuint));
	//fromInvertedY=fromWindowRect[kPsychBottom]-fromSampleRect[kPsychBottom];
	//PsychGetPrecisionTimerSeconds(&t1);
	glReadPixels(0,0, sourceWinWidth, sourceWinHeight, GL_BGRA , GL_UNSIGNED_INT_8_8_8_8_REV, textureMemory);
	//PsychGetPrecisionTimerSeconds(&t2);
	//mexPrintf("glReadPixels took %f seconds\n", t2-t1);
	PsychTestForGLErrors();
	CGLSetCurrentContext(NULL);		//clear the current context. 
      

    //Convert the bitmap which we created into into a texture held in "client storage" aka system memory.  
    PsychSetGLContext(destWinRec); 
    glEnable(GL_TEXTURE_RECTANGLE_EXT);
    glGenTextures(1, &myTexture);								//create an index "name" for our texture
    glBindTexture(GL_TEXTURE_RECTANGLE_EXT, myTexture);			//instantiate a texture of type associated with the index and set it to be the target for subsequent gl texture operators.
   //new
	//glTextureRangeAPPLE(GL_TEXTURE_RECTANGLE_EXT, 0, NULL); 
	glTextureRangeAPPLE(GL_TEXTURE_RECTANGLE_EXT, 1 * 1600 * 1200 * 4, textureMemory); 
	glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_STORAGE_HINT_APPLE , GL_STORAGE_SHARED_APPLE);
   //end new
	glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, 1);			//tell gl how to unpack from our memory when creating a surface, namely don't really unpack it but use it for texture storage.
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	//specify interpolation scaling rule for copying from texture.  
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //specify interpolation scaling rule from copying from texture.
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//PsychGetPrecisionTimerSeconds(&t1);
    glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA,  sourceWinWidth, sourceWinHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, textureMemory);
	//PsychGetPrecisionTimerSeconds(&t2);
	//mexPrintf("glTexImage2D took %f seconds\n", t2-t1);
	
    
    //Copy the texture to the display.  What are the s and  t indices  of the first pixel of the texture ? 0 or 1 ?
    //set the GL context to be the onscreen window
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);		//how to combine the texture with the target.  We could also alpha blend.
	PsychGetPrecisionTimerSeconds(&t1);
	    glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0);								glVertex2d(0.0, 0.0);
        glTexCoord2d(sourceWinWidth, 0.0 );					glVertex2d(sourceWinWidth, 0.0);
        glTexCoord2d(sourceWinWidth, sourceWinHeight);		glVertex2d(sourceWinWidth, sourceWinHeight);
        glTexCoord2d(0.0, sourceWinHeight);					glVertex2d(0.0, sourceWinHeight);
    glEnd();
    glFinish();
	PsychGetPrecisionTimerSeconds(&t2);
	mexPrintf("TestTextureTwo took %f seconds\n", t2-t1);
    glDisable(GL_TEXTURE_RECTANGLE_EXT);

    //Close  up shop.  Unlike with normal textures is important to release the context before deallocating the memory which glTexImage2D() was given. 
    //First release the GL context, then the CG context, then free the memory.
#endif

    return(PsychError_none);

}








