/*
	SCREENDrawTexture.c	

	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 

	PLATFORMS:

		Only OS X for now.

	HISTORY:
		
		6/25/04		awi		Created.
		10/12/04	awi		In useString: changed "SCREEN" to "Screen", and moved commas to inside [].
		1/25/05		awi		Added updated provided by mk
								- Screen-sized textures work correclty
								- Textures larger than the screen are rejected
								- Fixed sourerect parameter
		2/25/05		awi		Added call to PsychSetGLContext
							Added call to PsychUpdateAlphaBlendingFactorLazily().  Drawing now obeys settings by Screen('BlendFunction').
                4/22/05         mk              Commented out glFinish() - This would be a performance killer!
                5/13/05         mk              Support for rotated drawing of textures.
                7/23/05         mk              New options filterMode and globalAlpha. 
                9/30/05         mk              Remove size check for texturesize <= windowsize. This restriction doesn't apply anymore for new texture mapping code.
 
	DESCRIPTION:


	TO DO:  

		- Change the default target rect to be centered in the target window

		- Use  glTexSubImage2D instead of glTexImage2D so that when we specify the subrect of the source image we get left and top bounds ..wait, we don't need to do that
		because we do that with texcoord instead, right ?

		- Make sure that we are not inverted and fix it if so

		- Fix the close all windows on clear all problem

		- Write GetImageFromTexture or just overload GetImage so that it works for textures also. 

		- Test GetImage on the screen after drawing a texture.


*/


#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('DrawTexture', windowPointer, texturePointer [,sourceRect] [,destinationRect] [,rotationAngle] [, filterMode] [, globalAlpha]);";
//                                               1              2                3             4                5                6              7
static char synopsisString[] = 
	"Draw the texture specified via 'texturePointer' into the target window specified via 'windowPointer'. "
	"In the the OS X Psychtoolbox textures replace offscreen windows for fast drawing of images during animation."
        "'sourceRect' specifies a rectangular subpart of the texture to be drawn (Defaults to full texture). "
        "'destinationRect' defines the rectangular subpart of the window where the texture should be drawn. This defaults"
        "to centered on the screen. "
        "'rotationAngle' Specifies a rotation angle in degree for rotated drawing of the texture (Defaults to 0 deg. = upright). "
        "'filterMode' How to compute the pixel color values when the texture is drawn magnified, minified or drawn shifted, e.g., "
        "if sourceRect and destinationRect do not have the same size or if sourceRect specifies fractional pixel values. 0 = Nearest "
        "neighbour filtering, 1 = Bilinear filtering - this is the default. 'globalAlpha' A global alpha transparency value to apply "
        "to the whole texture for blending. Range is 0 = fully transparent to 1 = fully opaque, defaults to one. If both, an alpha-channel "
        "and globalAlpha are provided, then the final alpha is the product of both values.";
	  
static char seeAlsoString[] = "MakeTexture";

PsychError SCREENDrawTexture(void) 
{
	PsychWindowRecordType		*source, *target;
	PsychRectType			sourceRect, targetRect, tempRect;
	double rotationAngle = 0;   // Default rotation angle is zero deg. = upright.
        int filterMode = 1;         // Default filter mode is bilinear filtering.
        double globalAlpha = 1.0;   // Default global alpha is 1 == no effect.
        
    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //Get the window structure for the onscreen window.  It holds the onscreein GL context which we will need in the
    //final step when we copy the texture from system RAM onto the screen.
    PsychErrorExit(PsychCapNumInputArgs(7));   	
    PsychErrorExit(PsychRequireNumInputArgs(2)); 	
    PsychErrorExit(PsychCapNumOutputArgs(0)); 
	
    //Read in arguments
    PsychAllocInWindowRecordArg(1, kPsychArgRequired, &target);
    PsychAllocInWindowRecordArg(2, kPsychArgRequired, &source);
    if(source->windowType!=kPsychTexture) {
      PsychErrorExitMsg(PsychError_user, "The first argument supplied was a window pointer, not a texture pointer");
    }

    PsychCopyRect(sourceRect,source->rect);
    PsychCopyRect(targetRect,source->rect);
    PsychCopyInRectArg(3, kPsychArgOptional, sourceRect);
    if (IsPsychRectEmpty(sourceRect)) return(PsychError_none);


    if (target->stereomode==kPsychFreeFusionStereo || target->stereomode==kPsychFreeCrossFusionStereo) {
      // Special case for stereo: Only half the real window width:
      PsychMakeRect(&tempRect, target->rect[kPsychLeft],target->rect[kPsychTop],
		    target->rect[kPsychLeft] + PsychGetWidthFromRect(target->rect)/2,target->rect[kPsychBottom]);
    }
    else {
      // Normal case:
      PsychCopyRect(tempRect,target->rect);
    }
    
    PsychCenterRectInRect(sourceRect, tempRect, targetRect);
    PsychCopyInRectArg(4, kPsychArgOptional, targetRect);
    if (IsPsychRectEmpty(targetRect)) return(PsychError_none);

    PsychCopyInDoubleArg(5, kPsychArgOptional, &rotationAngle);
    PsychCopyInIntegerArg(6, kPsychArgOptional, &filterMode);
    if (filterMode<0 || filterMode>3) {
        PsychErrorExitMsg(PsychError_user, "filterMode needs to be 0 for nearest neighbour filter, or 1 for bilinear filter, or 2 for mipmapped filter or 3 for mipmapped-linear filter.");    
    }

    PsychCopyInDoubleArg(7, kPsychArgOptional, &globalAlpha);
    if (globalAlpha<0 || globalAlpha>1) {
        PsychErrorExitMsg(PsychError_user, "globalAlpha needs to be in range 0 for fully transparent to 1 for fully opaque.");    
    }
    
    PsychSetGLContext(target); 
    PsychUpdateAlphaBlendingFactorLazily(target);
    PsychBlitTextureToDisplay(source, target, sourceRect, targetRect, rotationAngle, filterMode, globalAlpha);	

    // Mark end of drawing op. This is needed for single buffered drawing:
    PsychFlushGL(target);

    return(PsychError_none);

}







