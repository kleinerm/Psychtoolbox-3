/*
  SCREENTestTexture.c	
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	All
    

  HISTORY:
  10/13/03  awi		Created.    
 
  DESCRIPTION:

  10/15/03
  
  Experimental SCREEN subfunction for testing out texture blitting using apple's AGP texture extension to get DMA transfers 
  from system RAM to VRAME over the AGP bus.  This should supplant the technique used by 'CopyWindow' now which is inelegant and slow.    
  
  Previously offscreen windows were created by passing CGLCreateContext() a block of memory and a pixel format.  The problem with
  CGLCreateContext() is that that there no fast way to transfer the contents of the context wich it creates to VRAM quickly because the packing
  format and alignment of the GL context created by CGLCreateContext() is opaque.  Using CGLCreateContext it not possible to create a GL 
  context in system RAM which was aligned with the packing format and alinment of VRAM.  The only way to access the images stored in the offscreen 
  context through glReadPixels wich copies from the the glContext memory to another block of applicatin memory where images whould are held in a 
  known format.  The method for copying from offscreen psychtoolbox windows to onscreen psychtoolbox windows was to allocated a temporary memory
  buffer the size of the copy block, use glReadPixels to copy the image from the offscreen context to the temporary buffer, then use glDrawPixels
  to copy from the temporary buffer to an onscreen glContext.  Well this is bad for several reasons:  it requires two copy operations, 
  it requires that the buffer be allocated, glReadPixels and glDrawPixels are slow because source and destinations are packed and aligned
  differently.  
  
  Geoff Stahl <gstahl@apple.com> suggested that I could used the combination of Apple GL  extensions  demonstrated in Apple's TextRange demo
  to get block DMA transfers directly from system RAM to video RAM.   We test that out here.  
  
  Now Psychtoolbox offscreen windows will be copied as GL textures instead of as offscreen gl contexts created using GLCreateContext().
  Usually when defining a texture using glTexImage2D() an application passes a pointer to a block of memory holding an image
  and GL copies the contents into its own block of texture memory, typically VRAM.  But with Appe's GL texture extensions,  an application
  passes glTexImage2D a pointer to a block of memory holding an image and GL instead of copinging the contents into a separate texture memory  
  GL retains the pointer and defines the texture to be the contents of that block of application memory specified to GLCreateContext().  The memory
  holding the texture thus has dual status, it is both a GL texture and it is a block of application memory.  
  
  Something unclear from Apple's documentation is how to declare that block of applciation memory to be a GL context if it will be given to
  glTexImage2D() to create a texture.  The TextureRange example does not demonstrate this.  GLCreateContext() will not work for this purpose because
  the pixel packing formats known to glTexImage2D() and GLCreateContext() functions must agree, yet the  GLCreateContext() pixel packing format is
  opaque.  
  
  It seems that CGBitmapContextCreate() unlike GLCreateContext() allows for specifying the packing format when creating the 
  GL rendering context, so 'TestTexture' uses CGBitmapContextCreate().  Note that Quartz will render text into a CGBitmapContext
  which probably how to implement Psychtoolbox's 'DrawText'.
  
  There are three levels of optimization necessary to obtain the DMA transfers and not all video boards support all three levels.
  This demo assumes all levels are available without checking.   It  should work with Radeaon 9000 on OS X 10.2.8. 
  
  10/16/03
  
  It turns out that there is no way to set the glContext to be a cgContext.  There are CGContextRef surfaces created with CGBitmapContextCreate() and there are
  CGLContextObj surfaces created with CGLCreateContext().  There is a way to set the gl context to a CGLContext object, CGLSetCurrentContext().  There is apparently 
  no way to set the gl context to a cgContextRef.  cgContextRef are for Quartz, CGLContextObj are for OpenGL.  I made the mistake and confusing the two.  The consequences
   are 1) This demo will only blit text and will not blit anything rendered with gl. This is still useful. 2) We either need to find a way to set assign a block of client 
   memory double status as both a glContext and a texture or else create two blocks of memory for offscreen windows, a CGContextRef which is the texture and CGLContextObj
   which is the GL rendering surface and we render into the CGLContextObj and copy it to the CGContextRef, which is the gl texture.  This is ugly and slower than ideal
   but only impacts the speed of creating and drawing into Psychtoolbox offscree windows, which is not time critical.  Finish the SCREENTestTexture demo, once that
   works post to the Mac gl mailing list and ask about this.   
  
  
  RESOURCES:
  Apple's TextureRange example.
  http://developer.apple.com/samplecode/Sample_Code/Graphics_3D/TextureRange.htm
  
  Core Graphcs API used to crete Core Graphics bitmap contexts.
    http://developer.apple.com/documentation/GraphicsImaging/Reference/Quartz_2D_Ref/index.html?http://developer.apple.com/documentation/GraphicsImaging/Reference/Quartz_2D_Ref/qref_main/function_group_1.html#//apple_ref/c/func/CGPDFContextCreate  
  

*/


#include "Screen.h"


static char useString[] = "copySpeed=SCREEN('TestTexture', windowPntr);";
static char synopsisString[] = 
	"Test out textures for implementing  offscreen windows.";
static char seeAlsoString[] = "";

//try to make the surface to double duty as a CG surface for quartz rendering surface and also as a GL surface. Otherwise poke the surface bits directly
//with specified fill values
#define useQuartz			1
#define redFill				0xFF
#define greenFill			0x00
#define blueFill			0X00
#define alphaFill			0XFF

//Specify the dimensions of the texture in units of texels.   GL's  power  rule (dimensions of a texture must be 2^n x 2^m, where n 
//and m are both positive integers) does not apply because the texture is GL_TEXTURE_RECTANGLE_EXTENSION and not GL_TEXTURE_2D.    
//#define textureSizeX			512
//#define textureSizeY			512
#define textureSizeX			600
#define textureSizeY			600


//Specify the string want to display and its position within the offscreen window/texture.
#define textPositionX			200
#define textPositionY			200  
#define textString			"Hello"

//Specify constants for creating the texture and for creating the CG context.  Conceptuall these are separate entities but 
//by use of Apple's GL extensions are embodied by the same block of RAM.  The pixel packing format of VRAM is fixed and from that follows 
//choices of packing formats for textures held in application memory and also the CG context emobied by the identical block of RAM.  

//Specify arguments to glTexImage2D when creating a texture to be held in client RAM. The choices are dictated  by our use of Apple's 
//GL_UNPACK_CLIENT_STORAGE_APPLE constant, an argument to glPixelStorei() which specifies the packing format of pixel the data passed to 
//glTexImage2D().  
#define texImage_target			GL_TEXTURE_2D
#define texImage_level			0
#define	texImage_internalFormat		GL_RGBA
#define texImage_sourceFormat		GL_BGRA
#define texImage_sourceType		GL_UNSIGNED_INT_8_8_8_8_REV

//Specify arguments to CGBitmapContextCreate() when creating a CG context which matches the pixel packing of the texture stored in client memory.
//The choice of values is dictated by our use arguments to    
#define cg_RGBA_32_BitsPerPixel		32
#define cg_RGBA_32_BitsPerComponent	8
#define cg_RGBA_32_AlphaOption		kCGImageAlphaPremultipliedLast


	 
PsychError SCREENTestTexture(void) 
{
#if PSYCH_SYSTEM == PSYCH_OSX

    PsychWindowRecordType 	*winRec;
    CGContextRef			cgContext;
    unsigned int			memoryTotalSizeBytes, memoryRowSizeBytes;
    UInt32					*textureMemory;
    int						stringLength, totalTexels, i;
    GLuint					myTexture;
    CGColorSpaceRef			cgColorSpace;
    
    			
    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //Get the window structure for the onscreen window.  It holds the onscreein GL context which we will need in the
    //final step when we copy the texture from system RAM onto the screen.
    PsychErrorExit(PsychCapNumInputArgs(1));   	
    PsychErrorExit(PsychRequireNumInputArgs(1)); 	
    PsychErrorExit(PsychCapNumOutputArgs(1));  
    PsychAllocInWindowRecordArg(1, TRUE, &winRec);
    if(!PsychIsOnscreenWindow(winRec))
        PsychErrorExitMsg(PsychError_user, "Onscreen window pointer required");
        
    //allocate memory for the surface
    memoryRowSizeBytes=sizeof(UInt32) * textureSizeX;
    memoryTotalSizeBytes= memoryRowSizeBytes * textureSizeY;
    textureMemory=(UInt32 *)malloc(memoryTotalSizeBytes);
    if(!textureMemory)
            PsychErrorExitMsg(PsychError_internal, "Failed to allocate surface memory\n");
    
    if(useQuartz){
        //Create the Core Graphics bitmap graphics context.  We have to be careful to specify arguments which will allow us to store the texture as an OpenGL texture. 
        //The choice of color space needs to be checked.  
        cgColorSpace=CGColorSpaceCreateDeviceRGB();
        cgContext= CGBitmapContextCreate(textureMemory, textureSizeX, textureSizeY, cg_RGBA_32_BitsPerComponent, memoryRowSizeBytes, cgColorSpace, cg_RGBA_32_AlphaOption);
        if(!cgContext){
            free((void *)textureMemory);
            PsychErrorExitMsg(PsychError_internal, "Failed to allocate CG Bimap Context\n");
        }
            
        //	Draw some text into the bitmap context.  We need to set font, size, pen (drawing mode), color, alpha, text position.
        
        //	There are two ways to select the font in a Core Graphics Quartz context depending on the type of font.
        //	1) CGContextSetFont() for Apple Type Services (ATS) font aka "The Right Way"
        //		A) call CGFontCreateWithPlatformFont() which returns a CGFontRef
        //		B) call CGContextSetFont() to set the font to be the drawing font within a context.
        //  2) CGContextSelectFont() for MacRoman aka "How We Do It"
        //
        //  Using MacRoman seems to mean that we just change the coding, though CGContextSelectFont().  For info on using ATS fonts see:
        // 	http://developer.apple.com/documentation/Carbon/Reference/ATS/
        CGContextSelectFont(cgContext, "Helvetica", (float)24, kCGEncodingMacRoman);		//set the font and its size.
        CGContextSetTextDrawingMode(cgContext, kCGTextFill);					//set the pen to be a filled pen
        CGContextSetRGBStrokeColor(cgContext, (float)0.5, (float)0.5, (float)0.0, (float)1.0);	//set the stroke color and alpha
        CGContextSetRGBFillColor(cgContext, (float)0.5, (float)0.5, (float)0.0, (float)1.0);	//set the fill color and alpha
        stringLength=strlen(textString);
        CGContextShowTextAtPoint(cgContext, (float)textPositionX, (float)textPositionY, textString, stringLength);	//draw at specified location.
        CGContextFlush(cgContext); 	//this might not be necessary but do it just in case.
    }else{
        //fill the texture memory by poking bits in the array which will be turned into a texture.
        totalTexels=textureSizeX * textureSizeY;
        for(i=0;i<totalTexels;i++)
            textureMemory[i]=	redFill << 24 | greenFill | 16 << blueFill << 8 | alphaFill;
     }       

    //Convert the CG graphics bitmap (Quartz surface) into a CG texture.  GL thinks we are loading the texture from memory we indicate to glTexImage2D, but really
    //we are just setting the texture to share the same memory as the Quartz surface.
    PsychSetGLContext(winRec); 
    glEnable(GL_TEXTURE_RECTANGLE_EXT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glGenTextures(1, &myTexture);						//create an index "name" for our texture
    glBindTexture(GL_TEXTURE_RECTANGLE_EXT, myTexture);			//instantiate a texture of type associated with the index and set it to be the target for subsequent gl texture operators.
    glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, 1);			//tell gl how to unpack from our memory when creating a surface, namely don't really unpack it but use it for texture storage.
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	//specify interpolation scaling rule for copying from texture.  
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //specify interpolation scaling rule from copying from texture.
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA,  textureSizeX, textureSizeY, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, textureMemory);
    
    //Copy the texture to the display.  What are the s and  t indices  of the first pixel of the texture ? 0 or 1 ?
    //set the GL context to be the onscreen window
    glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0);					glVertex2d(0.0, 0.0);
        glTexCoord2d(textureSizeX, 0.0 );			glVertex2d(textureSizeX, 0.0);
        glTexCoord2d(textureSizeX, textureSizeY);		glVertex2d(textureSizeX, textureSizeY);
        glTexCoord2d(0.0, textureSizeY);			glVertex2d(0.0, textureSizeY);
    glEnd();
    glFlush();	
    glDisable(GL_TEXTURE_RECTANGLE_EXT);

    //Close  up shop.  Unlike with normal textures is important to release the context before deallocating the memory which glTexImage2D() was given. 
    //First release the GL context, then the CG context, then free the memory.
    glDeleteTextures(1, &myTexture);	//Remove references from gl to the texture memory  & free gl's associated resources   
    if(useQuartz) CGContextRelease(cgContext);	//Remove references from Core Graphics to the texture memory & free Core Graphics' associated resources.
    free((void *)textureMemory);	//Free the memory
#endif
    return(PsychError_none);

}


	
	






