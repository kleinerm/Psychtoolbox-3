/*
 Common/Screen/SCREENSetOpenGLTexture.c		
 
 AUTHORS:
 mario.kleiner at tuebingen.mpg.de  mk
 
 PLATFORMS:	All
 
 
 HISTORY:
 02/07/06  mk		Created.
 
 DESCRIPTION:
 Takes a Psychtoolbox texture handle and updates all information that is needed to
 use or access the corresponding OpenGL texture with the provided information from
 an external OpenGL routine, e.g., moglcore or other mex-files...
 --> One can attach externally generated OpenGL textures to PTB's data structures,
 so PTB can use them as if they were created via 'MakeTexture' et al.
 
 */


#include "Screen.h"

static char useString[] = "Screen('SetOpenGLTexture', windowPtr, textureHandle, glTexid [, glWidth] [, glHeight] [, glDepth]);";
static char synopsisString[] = 
"Provides information about an external OpenGL texture to make it acessible for PTB as PTB texture."
"\"windowPtr\" is the handle of the onscreen window for which texture should be attached. "
"'textureHandle' is the Psychtoolbox handle for the requested texture. glTexid is the OpenGL texture "
"id of a valid OpenGL texture object. Optionally you can pass in the intended width and height as well "
"as pixeldepth of the texture if they should be different from the values that PTB can autodetect from "
"the given texture object. This routine allows external OpenGL code to inject textures into PTB for use "
"with it. For more info about OpenGL textures, read an OpenGL book. ";

static char seeAlsoString[] = "GetOpenGLTexture";

PsychError SCREENSetOpenGLTexture(void) 
{
    PsychWindowRecordType *windowRecord, *textureRecord;
    int texid, w, h, d;
    texid=w=h=d=-1;
    GLenum target = 0;
    GLboolean flag=FALSE;
    
    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    PsychErrorExit(PsychCapNumInputArgs(6));     //The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(3)); //The required number of inputs	
    PsychErrorExit(PsychCapNumOutputArgs(0));    //The maximum number of outputs
    
    // Get the window record from the window record argument and get info from the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
    
    // Get the texture record from the texture record argument.
    PsychAllocInWindowRecordArg(2, TRUE, &textureRecord);
    if (!PsychIsTexture(textureRecord)) {
        PsychErrorExitMsg(PsychError_user, "You tried to set texture information on something else than a texture!");
    }
    
    // Query texid:
    PsychCopyInIntegerArg(3, TRUE, &texid);
    
    // Query optional width:
    PsychCopyInIntegerArg(4, FALSE, &w);

    // Query optional height:
    PsychCopyInIntegerArg(5, FALSE, &h);

    // Query optional depth:
    PsychCopyInIntegerArg(6, FALSE, &d);

    // Activate OpenGL rendering context of windowRecord and make it the active drawing target:
    PsychSetGLContext(windowRecord);
    PsychSetDrawingTarget(windowRecord);
    
    // Query texture target type:
    glGetBooleanv(GL_TEXTURE_2D, &flag);
    if (flag) target=GL_TEXTURE_2D;
    glGetBooleanv(GL_TEXTURE_RECTANGLE_EXT, &flag);
    if (flag) target=GL_TEXTURE_RECTANGLE_EXT;
    glGetBooleanv(GL_TEXTURE_RECTANGLE_NV, &flag);
    if (flag) target=GL_TEXTURE_RECTANGLE_NV;
    if (target==0) {
        PsychErrorExitMsg(PsychError_invalidArg_type, "Provided texture is not one of the supported types GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_EXT or GL_TEXTURE_RECTANGLE_NV");
    }
    
    // Bind it:
    glBindTexture(target, texid);
    PsychTestForGLErrors();
    
    // Binding worked. Query its size and format:
    // AND SO ON....
    
    glBindTexture(target, 0);
    
    // Done.
    return(PsychError_none);
}
