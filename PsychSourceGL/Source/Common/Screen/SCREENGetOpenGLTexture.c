/*
  Common/Screen/SCREENGetOpenGLTexture.c		
  
  AUTHORS:
  mario.kleiner at tuebingen.mpg.de  mk
  
  PLATFORMS:	All
    

  HISTORY:
  02/07/06  mk		Created.
 
  DESCRIPTION:
  Takes a Psychtoolbox texture handle and returns all information that is needed to
  use or access the corresponding OpenGL texture with an external OpenGL routine,
  e.g., moglcore or other mex-files...
 
 */


#include "Screen.h"

static char useString[] = "[ gltexid gltextarget texcoord_u texcoord_v ] =Screen('GetOpenGLTexture', windowPtr, textureHandle [, x][, y]);";
static char synopsisString[] = 
	"Returns information about the OpenGL texture corresponding to a Psychtoolbox texture. "
	"\"windowPtr\" is the handle of the onscreen window for which texture should be returned. "
        "'textureHandle' is the Psychtoolbox handle for the requested texture. Optionally you can "
        "pass in Psychtoolbox texture coordinates (x,y) and get them mapped to the proper OpenGL "
        "texture coordinates. Return values: 'gltexid' OpenGL texture id for binding the texture. "
        "'gltextarget' type of OpenGL texture target to use. 'texcoord_u' and 'texcoord_v' "
        "OpenGL texture coordinates corresponding to 'x' and 'y'. Immediately after calling this "
        "routine, the proper OpenGL rendering context for the requested texture is activated for use. "
        "Example of usage: glBindTexture(gltextarget, gltexid); // Activate and bind req. texture. "
        "glTexCoord2d(texcoord_u, texcoord_v); // Assign texture pixel (x,y) to next vertex. For more "
        "info, read an OpenGL book. ";

static char seeAlsoString[] = "SetOpenGLTexture";

PsychError SCREENGetOpenGLTexture(void) 
{
    PsychWindowRecordType *windowRecord, *textureRecord;
    double                x,y;
    x=y=-1;
    
    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    PsychErrorExit(PsychCapNumInputArgs(4));     //The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(2)); //The required number of inputs	
    PsychErrorExit(PsychCapNumOutputArgs(4));    //The maximum number of outputs
    
    // Get the window record from the window record argument and get info from the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);

    // Get the texture record from the texture record argument.
    PsychAllocInWindowRecordArg(2, TRUE, &textureRecord);
    if (!PsychIsTexture(textureRecord)) {
        PsychErrorExitMsg(PsychError_user, "You tried to query texture information on something else than a texture!");
    }
    
    // Query optional x-pos:
    PsychCopyInDoubleArg(3, FALSE, &x);
 
    // Query optional y-pos:
    PsychCopyInDoubleArg(4, FALSE, &y);

    // Return the OpenGL texture handle:
    PsychCopyOutDoubleArg(1, FALSE, (double) textureRecord->textureNumber);
    
    // Return type of texture target:
    PsychCopyOutDoubleArg(2, FALSE, (double) PsychGetTextureTarget(textureRecord));
    
    // If no texcoords provided, we'll let them default to full texture size:
    if (x==-1) x=PsychGetWidthFromRect(textureRecord->rect);
    if (y==-1) y=PsychGetHeightFromRect(textureRecord->rect);

    // Remap texture coordinates:
    PsychMapTexCoord(textureRecord, &x, &y);
        
    // Return mapped texture coordinates:
    PsychCopyOutDoubleArg(3, FALSE, x);
    PsychCopyOutDoubleArg(4, FALSE, y);
    
    // Done.
    return(PsychError_none);
}

