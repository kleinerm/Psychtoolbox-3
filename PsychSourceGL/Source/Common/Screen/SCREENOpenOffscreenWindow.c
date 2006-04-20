/*

	SCREENOpenOffscreenWindow.c		

	AUTHORS:

		Allen.Ingling@nyu.edu               awi 
                mario.kleiner at tuebingen.mpg.de   mk
  
	PLATFORMS:	

        Platform independent, but requires OS-X >= 10.4.3 or a M$-Windows system with support for
        OpenGL Framebuffer objects (OpenGL 1.5 with extensions or OpenGL 2.x)
 
	HISTORY:



		1/18/02		awi		Created.  Copied the Synopsis string from old version of psychtoolbox.
		2/18/03		awi		Set the screen number for all offscreen windows to -1.   
							What use does the screen number serve for offscren windows ?
							¥Because we use glDrawPixels and glReadPixels to move pixels to and from surfaces, and those handle 
							alignment on the surface, knowing the parent window is not necessary for the purpose of matching 
							alignments in copy operations. 
							¥The bit depths do need to match for CopyWindow, and we could use the parent window as an indicator of 
							matching depth.  However, if we want to match depths, it is better to compare depths than to use 
							the parent window number as a proxy for depth; It is more transparent and more general because it handles copying 			
							between two onscreen windows of equal depth but which do not share the same parent window.

		10/12/04	awi		In useString: moved commas to inside [].
		2/15/05		awi		Commented out glEnable(GL_BLEND) 
                1/16/06         mk              Rewritten to initialize offscreen windows as if they were textures with a constant colored background instead of content.
 
	TO DO:

		Find out if the window background color is supposed to be always white,  or the color of the parent window.
*/

#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] =  "[windowPtr,rect]=Screen('OpenOffscreenWindow',windowPtrOrScreenNumber [,color] [,rect] [,pixelSize]);";
//                                                                        1                         2        3       4                  

static char synopsisString[] =
    "Open an offscreen window. This is simply an OpenGL texture that is treated "
    "as a window, so you can draw to it. Offscreen windows should be only used "
    "to keep old code from OS-9 Psychtoolbox working or if you need to draw to them "
    "fast. For quickly displaying images, please use the 'MakeTexture' and 'DrawTexture' "
    "commands. They allow for significantly higher drawing speeds. You can specify a screen "
    "(any windowPtr or a screenNumber>=0) or no screen(screenNumber=-1), but any real screen "
    "must already have an open Screen window when you call OpenOffscreenWindow. "
    "\"color\" is the clut index (scalar or [r g b] triplet or [r g b a] quadruple) that you want to poke into "
    "each pixel as initial background color; default is white. \"rect\" specifies the size of the offscreen window "
    "If supplied, \"rect\" must contain at least one pixel. If a windowPtr is supplied, "
    "then \"rect\" defaults to the whole window. "
    "If a screenNumber is supplied then \"rect\" defaults to the whole screen. If a screenNumber of "
    "-1 is supplied, then \"rect\" defaults to the size of the main screen. (In all "
    "cases, subsequent references to this new offscreen window will use its "
    "coordinates: origin at its upper left.) \"pixelSize\" sets the depth (in bits) of "
    "each pixel. If you specify no screen (screenNumber=-1) then the "
    "default pixelSize is 32, but you can specify any legal depth: 8, 16, 24, 32. "
    "A pixelSize of 0 or [] is replaced by the default of 32 bits per pixel. If you specify "
    "a screen number of windowPtr, then the default depth is that of the screen or window. "
    "NOTE: Screen's windows are known only to Screen and must be closed by it, e.g., "
    "Screen(w,'Close'). Matlab knows nothing about Screen's windows, so the Matlab "
    "CLOSE command won't work on Screen's windows. ";
static char seeAlsoString[] = "OpenWindow";

PsychError SCREENOpenOffscreenWindow(void) 
{
    int					screenNumber, depth, targetScreenNumber;
    PsychRectType			rect;
    PsychColorType			color;
    PsychColorModeType  		mode; 
    boolean				didWindowOpen;
    PsychWindowRecordType		*exampleWindowRecord, *windowRecord, *targetWindow;
    Boolean				wasColorSupplied;
    char*                               texturePointer;
    int                                 xSize, ySize, nbytes;
    
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    //cap the number of inputs
    PsychErrorExit(PsychCapNumInputArgs(5));   //The maximum number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(2));  //The maximum number of outputs

    //1-User supplies a window ptr 2-User supplies a screen number 3-User supplies rect and pixel size
    if(PsychIsWindowIndexArg(1)){
        PsychAllocInWindowRecordArg(1, TRUE, &exampleWindowRecord);
        PsychCopyRect(rect, exampleWindowRecord->rect);
        // We copy depth only from exampleWindow if it is a offscreen window (=texture). Copying from
        // onscreen windows doesn't make sense, e.g. depth=16 for onscreen means RGBA8 window, but it
        // would map onto a LUMINANCE+ALPHA texture for the offscreen window! We always use 32 bit RGBA8
        // in such a case.
        depth=(PsychIsOffscreenWindow(exampleWindowRecord)) ? exampleWindowRecord->depth : 32;
        targetScreenNumber=exampleWindowRecord->screenNumber;
        targetWindow=exampleWindowRecord;
    } else if(PsychIsScreenNumberArg(1)){
        PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);
        PsychGetScreenRect(screenNumber, rect);
        depth=32; // Always use RGBA8 in this case! See above...
        targetScreenNumber=screenNumber;
        targetWindow=NULL;
    } else if(PsychIsUnaffiliatedScreenNumberArg(1)){  //that means -1 or maybe also NaN if we add that option.  
        // Default to a depth of 32 bpp:
        depth=32;
        targetScreenNumber=PSYCH_FIRST_SCREEN; // We assign the first screen in the system.
        PsychGetScreenRect(targetScreenNumber, rect);
        targetWindow=NULL;
    } else {
        PsychErrorExit(PsychError_invalidNumdex);
    }

    if (targetWindow==NULL) {
        // Get target window of screen:
        PsychFindScreenWindowFromScreenNumber(targetScreenNumber, &targetWindow);
    }
    
    //Depth and rect argument supplied as arguments override those inherited from reference screen or window.
    //Note that PsychCopyIn* prefix means that value will not be overwritten if the arguments are not present.
    PsychCopyInRectArg(3,FALSE, rect);
    PsychCopyInIntegerArg(4,FALSE, &depth); 

    // If any of the no longer supported values 0, 1, 2 or 4 is provided, we
    // silently switch to 32 bits per pixel, which is the safest and fastest setting:
    if (depth==0 || depth==1 || depth==2 || depth==4) depth=32;

    // Final sanity check:
    if (depth!=8 && depth!=16 && depth!=24 && depth!=32) {
      PsychErrorExitMsg(PsychError_user, "Invalid depth value provided. Must be 8 bpp, 16 bpp, 24 bpp or 32 bpp!");
    }

    //find the color for the window background.  
    wasColorSupplied=PsychCopyInColorArg(kPsychUseDefaultArgPosition, FALSE, &color); //get from user
    if(!wasColorSupplied) PsychLoadColorStruct(&color, kPsychIndexColor, PsychGetWhiteValueFromDepthValue(32)); // Use the default 32 bpp!!!  
    PsychCoerceColorModeFromSizes(PsychGetNumPlanesFromDepthValue(32), PsychGetColorSizeFromDepthValue(32), &color);
    // printf("R=%i G=%i B=%i A=%i I=%i", color.value.rgba.r, color.value.rgba.g,color.value.rgba.b,color.value.rgba.a,color.value.index); 
    // First allocate the offscreen window record to store stuff into. If we exit with an error PsychErrorExit() should
    // call PsychPurgeInvalidWindows which will clean up the window record. 
    PsychCreateWindowRecord(&windowRecord);  // This also fills the window index field.
    // This offscreen window is implemented as a Psychtoolbox texture:
    windowRecord->windowType=kPsychTexture;
    // We need to assign the screen number of the onscreen-window, so PsychCreateTexture()
    // can query the size of the screen/onscreen-window...
    windowRecord->screenNumber = targetScreenNumber;
    // Assign the computed depth:
    windowRecord->depth=depth;
    // Assign the computed rect:
    PsychCopyRect(windowRecord->rect, rect);
    // Allocate the texture memory:
    // We only allocate the amount really needed for given format, aka numMatrixPlanes - Bytes per pixel.
    xSize = PsychGetWidthFromRect(rect);
    ySize = PsychGetHeightFromRect(rect);
    windowRecord->textureMemorySizeBytes = (depth/8) * xSize * ySize;
    windowRecord->textureMemory = malloc(windowRecord->textureMemorySizeBytes);
    texturePointer=(char*) windowRecord->textureMemory;
    // printf("depth=%i xsize=%i ysize=%i mem=%i ptr=%p", depth, xSize, ySize, windowRecord->textureMemorySizeBytes, texturePointer);
    // Fill with requested background color:
    nbytes=0;
    switch (depth) {
        case 8: // Pure LUMINANCE texture:
            memset((void*) texturePointer, color.value.rgba.r, windowRecord->textureMemorySizeBytes);
        break;
    
        case 16: // LUMINANCE + ALPHA
            while (nbytes < windowRecord->textureMemorySizeBytes) {
                *(texturePointer++) = color.value.rgba.r;
                *(texturePointer++) = color.value.rgba.a;
                nbytes+=2;
            }
        break;
    
        case 24: // RGB:
            while (nbytes < windowRecord->textureMemorySizeBytes) {
                *(texturePointer++) = color.value.rgba.r;
                *(texturePointer++) = color.value.rgba.g;
                *(texturePointer++) = color.value.rgba.b;
                nbytes+=3;
            }
        break;        
    
        case 32: // RGBA
            while (nbytes < windowRecord->textureMemorySizeBytes) {
                *(texturePointer++) = color.value.rgba.a;
                *(texturePointer++) = color.value.rgba.r;
                *(texturePointer++) = color.value.rgba.g;
                *(texturePointer++) = color.value.rgba.b;
                nbytes+=4;
            }
        break;
    }
    
    // Setup associated OpenGL context:
    windowRecord->targetSpecific.contextObject = targetWindow->targetSpecific.contextObject;
	 windowRecord->targetSpecific.deviceContext = targetWindow->targetSpecific.deviceContext;

    // Texture orientation is type 2 aka upright, non-transposed aka Offscreen window:
    windowRecord->textureOrientation = 2;
    
    // Let's create and bind a new texture object and fill it with our new texture data.
    PsychCreateTexture(windowRecord);
    
    // Window ready. Mark it valid and return handle to userspace:
    PsychSetWindowRecordValid(windowRecord);
    
    //Return the window index and the rect argument.
    PsychCopyOutDoubleArg(1, FALSE, windowRecord->windowIndex);
    PsychCopyOutRectArg(2, FALSE, rect);

    // Ready.
    return(PsychError_none);
}
