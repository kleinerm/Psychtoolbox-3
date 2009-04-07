/*
    SCREENDrawText.c	
  
    AUTHORS:
    
		Allen.Ingling@nyu.edu					awi
		mario.kleiner at tuebingen.mpg.de	mk
  
    PLATFORMS:
		
		All.
    
    HISTORY:
	
		11/17/03	awi		Spun off from SCREENTestTexture which also used Quartz and Textures to draw text but did not match the 'DrawText' specifications.
		10/12/04	awi		In useString: changed "SCREEN" to "Screen", and moved commas to inside [].
		2/25/05		awi		Added call to PsychUpdateAlphaBlendingFactorLazily().  Drawing now obeys settings by Screen('BlendFunction').
		5/08/05     mk      Bugfix for "Descenders of letters get cut/eaten away" bug introduced in PTB 1.0.5
		10/12/05    mk      Fix crash in DrawText caused by removing glFinish() while CLIENT_STORAGE is enabled!
							-> Disabling CLIENT_STORAGE and removing glFinish() is the proper solution...
		11/01/05    mk      Finally the real bugfix for "Descenders of letters get cut/eaten away" bug introduced in PTB 1.0.5!
		11/01/05    mk      Removal of dead code + beautification.
		11/21/05    mk      Code for updating the "Drawing Cursor" and returning NewX, NewY values added.
		01/01/06    mk      Code branch for M$-Windoze implementation of DrawText added.
		11/11/07		mk		  New GDI based Windoze text renderer implemented.

    DESCRIPTION:

		Unified file with text renderers for all platforms (OS/X, Windows, Linux).
  
    REFERENCES:
	
		http://oss.sgi.com/projects/ogl-sample/registry/APPLE/client_storage.txt
		http://developer.apple.com/samplecode/Sample_Code/Graphics_3D/TextureRange.htm
  
    TO DO:

		Platform specific code should be in the platform folders, not here in the Common folder! Sort this out some time.
		This file is in need for extensive cleanup and refactoring!!!
							
*/


#include "Screen.h"


// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.
static char useString[] = "[newX,newY]=Screen('DrawText', windowPtr, text [,x] [,y] [,color] [,backgroundColor] [,yPositionIsBaseline]);";
static char seeAlsoString[] = "";

#if PSYCH_SYSTEM == PSYCH_OSX

#define CHAR_TO_UNICODE_LENGTH_FACTOR		4			//Apple recommends 3 or 4 even though 2 makes more sense.
#define USE_ATSU_TEXT_RENDER			1

static char synopsisString[] = 
    "Draw text. \"text\" may include two-byte (16 bit) Unicode characters (e.g. Chinese). "
	"A standard Matlab/Octave text string is interpreted as 8 bit ASCII string. If you "
	"want to pass a string which contains 16 bit UTF-16 unicode characters, convert the "
	"text to a double matrix, ie, mytext = double(myunicodetext); then pass the double "
	"matrix to this function.\n"
    "Default \"x\" \"y\" is current pen location. \"color\" is the CLUT index (scalar or [r "
    "g b] triplet) that you want to poke into each pixel; default produces black with "
    "the standard CLUT for this window's pixelSize. \"backgroundColor\" is the color of "
	"the text background. \"yPositionIsBaseline\" If specified, will override the global "
	"preference setting for text positioning: It defaults to off. If it is set to 1, the y "
	"pen location defines the base line of drawn text, otherwise it defines the top of the "
	"drawn text. Old PTB's had a behaviour equivalent to setting 1, unfortunately this behaviour "
	"wasn't replicated in pre 3.0.8 PTB's so now we stick to the new behaviour by default. "
	"\"newX, newY\" return the final pen location.\n"
	"Btw.: Screen('Preference', ...); provides a couple of interesting text preference "
	"settings that affect text drawing, e.g., setting alpha blending and anti-aliasing modes.";

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

PsychError SCREENDrawText(void) 
{
	char			errmsg[1000];
    PsychWindowRecordType 	*winRec;
    CGContextRef		cgContext;
    unsigned int		memoryTotalSizeBytes, memoryRowSizeBytes;
    UInt32			*textureMemory;
    GLuint			myTexture;
    CGColorSpaceRef		cgColorSpace;
    PsychRectType		windowRect;
    //	double			textureSizeX, textureSizeY;
    char			*textString;
    Boolean			doSetColor, doSetBackgroundColor;
    PsychColorType		colorArg, backgroundColorArg;
    //	CGImageAlphaInfo	quartzAlphaMode, correctQuartzAlphaMode, correctQuartzAlphaModeMaybe;
    CGRect			quartzRect;
    GLdouble			backgroundColorVector[4];
    //for creating the layout object and setting the run attributes.  (we can get rid of these after we unify parts of DrawText and TextBounds).
    char			*textCString;
    int				stringLengthChars;
    Str255			textPString;
    int				uniCharBufferLengthElements, uniCharBufferLengthChars, uniCharBufferLengthBytes;
    UniChar			*textUniString;
    TextEncoding                textEncoding;
    OSStatus			callError;
    TextToUnicodeInfo		textToUnicodeInfo;
    ByteCount			uniCharStringLengthBytes;
    ATSUStyle			atsuStyle;
    ATSUTextLayout		textLayout;				//layout is a pointer to an opaque struct.
    
    Rect			textBoundsQRect;
    double			textBoundsPRect[4], textBoundsPRectOrigin[4], textureRect[4];
    double			textureWidth, textureHeight, textHeight, textWidth, textureTextFractionY, textureTextFractionXLeft,textureTextFractionXRight, textHeightToBaseline;
    double			quadLeft, quadRight, quadTop, quadBottom;
    int				yPositionIsBaseline;
    GLenum			normalSourceBlendFactor, normalDestinationBlendFactor;
	int				dummy1, dummy2;
	double*			unicodedoubles;
	int ix;
	GLubyte* rpb;
	Boolean bigendian;
	
	// Detect endianity (byte-order) of machine:
    ix=255;
    rpb=(GLubyte*) &ix;
    bigendian = ( *rpb == 255 ) ? FALSE : TRUE;
    ix = 0; rpb = NULL;
	textCString = NULL;
    
    //for layout attributes.  (not the same as run style attributes set by PsychSetATSUTStyleAttributes or line attributes which we do not set.) 	
    ATSUAttributeTag		saTags[] =  {kATSUCGContextTag };
    ByteCount			saSizes[] = {sizeof(CGContextRef)};
    ATSUAttributeValuePtr       saValue[] = {&cgContext};
    
    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //Get the window structure for the onscreen window.  It holds the onscreein GL context which we will need in the
    //final step when we copy the texture from system RAM onto the screen.
    PsychErrorExit(PsychCapNumInputArgs(7));   	
    PsychErrorExit(PsychRequireNumInputArgs(2)); 	
    PsychErrorExit(PsychCapNumOutputArgs(2));  
    PsychAllocInWindowRecordArg(1, TRUE, &winRec);
    
    //Get the dimensions of the target window
    PsychGetRectFromWindowRecord(windowRect, winRec);
    
    //Get the text string (it is required)
    //PsychAllocInCharArg(2, kPsychArgRequired, &textString);
    
    //Get the X and Y positions.
    PsychCopyInDoubleArg(3, kPsychArgOptional, &(winRec->textAttributes.textPositionX));
    PsychCopyInDoubleArg(4, kPsychArgOptional, &(winRec->textAttributes.textPositionY));
    
    //Get the color
    //Get the new color record, coerce it to the correct mode, and store it.  
    doSetColor=PsychCopyInColorArg(5, kPsychArgOptional, &colorArg);
    if(doSetColor) PsychSetTextColorInWindowRecord(&colorArg,  winRec);
    
    doSetBackgroundColor=PsychCopyInColorArg(6, kPsychArgOptional, &backgroundColorArg);
    if(doSetBackgroundColor) PsychSetTextBackgroundColorInWindowRecord(&backgroundColorArg,  winRec);
    
    /////////////common to TextBounds and DrawText:create the layout object////////////////// 
    //read in the string and get its length and convert it to a unicode string.
    if (PsychGetArgType(2) == PsychArgType_char) {
		PsychAllocInCharArg(2, TRUE, &textCString);
		stringLengthChars=strlen(textCString);
		if(stringLengthChars < 1) goto drawtext_skipped; // We skip most of the code if string is empty.
		if(stringLengthChars > 255) PsychErrorExitMsg(PsychError_unimplemented, "Cut corners and TextBounds will not accept a string longer than 255 characters");
		CopyCStringToPascal(textCString, textPString);
		uniCharBufferLengthChars= stringLengthChars * CHAR_TO_UNICODE_LENGTH_FACTOR;
		uniCharBufferLengthElements= uniCharBufferLengthChars + 1;		
		uniCharBufferLengthBytes= sizeof(UniChar) * uniCharBufferLengthElements;
		textUniString=(UniChar*)malloc(uniCharBufferLengthBytes);
		//Using a TextEncoding type describe the encoding of the text to be converteed.  
		textEncoding=CreateTextEncoding(kTextEncodingMacRoman, kMacRomanDefaultVariant, kTextEncodingDefaultFormat);
		//Create a structure holding conversion information from the text encoding type we just created.
		callError=CreateTextToUnicodeInfoByEncoding(textEncoding,&textToUnicodeInfo);
		//Convert the text to a unicode string
		callError=ConvertFromPStringToUnicode(textToUnicodeInfo, textPString, (ByteCount)uniCharBufferLengthBytes,	&uniCharStringLengthBytes,	textUniString);
	}
	else {
		// Not a character string: Check if it's a double matrix for Unicode text encoding:
		PsychAllocInDoubleMatArg(2, TRUE, &dummy1, &stringLengthChars, &dummy2, &unicodedoubles);
		if (dummy1!=1 || dummy2!=1) PsychErrorExitMsg(PsychError_user, "Unicode text matrices must be 1 row by character columns!");
		if(stringLengthChars < 1) goto drawtext_skipped; // We skip most of the code if string is empty.		
		textUniString=(UniChar*) malloc(sizeof(UniChar) * stringLengthChars);
		for (dummy1=0; dummy1<stringLengthChars; dummy1++) textUniString[dummy1] = (UniChar) unicodedoubles[dummy1];
	}
	
	//create the text layout object
    callError=ATSUCreateTextLayout(&textLayout);
    //associate our unicode text string with the text layout object
    callError=ATSUSetTextPointerLocation(textLayout, textUniString, kATSUFromTextBeginning, kATSUToTextEnd, (UniCharCount)stringLengthChars);
    //create an ATSU style object and tie it to the layout object in a style run.
    callError=ATSUCreateStyle(&atsuStyle);
    callError=ATSUClearStyle(atsuStyle);
    PsychSetATSUStyleAttributesFromPsychWindowRecord(atsuStyle, winRec);
    callError=ATSUSetRunStyle(textLayout, atsuStyle, (UniCharArrayOffset)0, (UniCharCount)stringLengthChars);
    /////////////end common to TextBounds and DrawText//////////////////
    
	// Define the meaning of the y position of the specified drawing cursor.
	// We get the global setting from the Screen preference, but allow to override
	// it on a per-invocation basis via the optional 7th argument to 'DrawText':
	yPositionIsBaseline = PsychPrefStateGet_TextYPositionIsBaseline();
	PsychCopyInIntegerArg(7, kPsychArgOptional, &yPositionIsBaseline);
	 
	if (yPositionIsBaseline) {
		// Y position of drawing cursor defines distance between top of text and
		// baseline of text, i.e. the textheight excluding descenders of letters:

		// Need to compute offset via ATSU:
		ATSUTextMeasurement mleft, mright, mtop, mbottom;
        callError=ATSUGetUnjustifiedBounds(textLayout, kATSUFromTextBeginning, kATSUToTextEnd, &mleft, &mright, &mbottom, &mtop);
		if (callError) {
			PsychErrorExitMsg(PsychError_internal, "Failed to compute unjustified text height to baseline in call to ATSUGetUnjustifiedBounds().\n");    
		}

		// Only take height including ascenders into account, not the descenders.
		// MK: Honestly, i have no clue why this is the correct calculation (or if it is
		// the correct calculation), but visually it seems to provide the correct results
		// and i'm not a typographic expert and don't intend to become one...
		textHeightToBaseline = fabs(Fix2X(mbottom));
	}
	else {
		// Y position of drawing cursor defines top of text, therefore no offset (==0) needed:
		textHeightToBaseline = 0;
	}

    //Get the bounds for our text so that and create a texture of sufficient size to containt it. 
    ATSTrapezoid trapezoid;
    ItemCount oActualNumberOfBounds = 0;
    callError=ATSUGetGlyphBounds(textLayout, 0, 0, kATSUFromTextBeginning, kATSUToTextEnd, kATSUseDeviceOrigins, 0, NULL, &oActualNumberOfBounds);
    if (callError || oActualNumberOfBounds!=1) {
        PsychErrorExitMsg(PsychError_internal, "Failed to compute bounding box in call 1 to ATSUGetGlyphBounds() (nrbounds!=1)\n");    
    }
    callError=ATSUGetGlyphBounds(textLayout, 0, 0, kATSUFromTextBeginning, kATSUToTextEnd, kATSUseDeviceOrigins, 1, &trapezoid, &oActualNumberOfBounds);
    if (callError || oActualNumberOfBounds!=1) {
        PsychErrorExitMsg(PsychError_internal, "Failed to retrieve bounding box in call 2 to ATSUGetGlyphBounds() (nrbounds!=1)\n");    
    }
    
    textBoundsPRect[kPsychLeft]=(Fix2X(trapezoid.upperLeft.x) < Fix2X(trapezoid.lowerLeft.x)) ? Fix2X(trapezoid.upperLeft.x) : Fix2X(trapezoid.lowerLeft.x);
    textBoundsPRect[kPsychRight]=(Fix2X(trapezoid.upperRight.x) > Fix2X(trapezoid.lowerRight.x)) ? Fix2X(trapezoid.upperRight.x) : Fix2X(trapezoid.lowerRight.x);
    textBoundsPRect[kPsychTop]=(Fix2X(trapezoid.upperLeft.y) < Fix2X(trapezoid.upperRight.y)) ? Fix2X(trapezoid.upperLeft.y) : Fix2X(trapezoid.upperRight.y);
    textBoundsPRect[kPsychBottom]=(Fix2X(trapezoid.lowerLeft.y) > Fix2X(trapezoid.lowerRight.y)) ? Fix2X(trapezoid.lowerLeft.y) : Fix2X(trapezoid.lowerRight.y);
    
    // printf("Top %lf x Bottom %lf :: ",textBoundsPRect[kPsychTop], textBoundsPRect[kPsychBottom]); 
    PsychNormalizeRect(textBoundsPRect, textBoundsPRectOrigin);
    // printf("N: Top %lf x Bottom %lf :: ",textBoundsPRectOrigin[kPsychTop], textBoundsPRectOrigin[kPsychBottom]);
	// Denis found an off-by-one bug in the text width. Don't know where it should come from in our code, but
	// my "solution" is to simply extend the width by one: 
    textWidth=PsychGetWidthFromRect(textBoundsPRectOrigin) + 1.0;
    textHeight=PsychGetHeightFromRect(textBoundsPRectOrigin);
    // printf("N: Width %lf x Height %lf :: ", textWidth, textHeight); 
    PsychFindEnclosingTextureRect(textBoundsPRectOrigin, textureRect);
    //Allocate memory the size of the texture.  The CG context is the same size.  It could be smaller, because Core Graphics surfaces don't have the power-of-two
    //constraint requirement.   
    textureWidth=PsychGetWidthFromRect(textureRect);
    textureHeight=PsychGetHeightFromRect(textureRect);
    memoryRowSizeBytes=sizeof(UInt32) * textureWidth;
    memoryTotalSizeBytes= memoryRowSizeBytes * textureHeight;
    textureMemory=(UInt32 *)valloc(memoryTotalSizeBytes);
    if(!textureMemory) PsychErrorExitMsg(PsychError_internal, "Failed to allocate surface memory\n");
    // printf("N: TexWidth %lf x TexHeight %lf :: ", textureWidth, textureHeight); 
    
    //Create the Core Graphics bitmap graphics context.  We can tell CoreGraphics to use the same memory storage format as will our GL texture, and in fact use
    //  the idential memory for both.   
    cgColorSpace=CGColorSpaceCreateDeviceRGB();
    //there is a bug here.  the format constant should be ARGB not RBGA to agree with the texture format.           
    cgContext= CGBitmapContextCreate(textureMemory, textureWidth, textureHeight, 8, memoryRowSizeBytes, cgColorSpace, kCGImageAlphaPremultipliedFirst);
    if(!cgContext){
        free((void *)textureMemory);
		printf("PTB-ERROR: In Screen('DrawText'): Failed to allocate CG Bitmap Context for: texWidth=%i, texHeight=%i, memRowSize=%i, textCString=%s\n", textureWidth, textureHeight, memoryRowSizeBytes, textCString);
		printf("PTB-ERROR: In Screen('DrawText'): xPos=%lf yPos=%lf StringLength=%i\nDecoded Unicode-String:\n", winRec->textAttributes.textPositionX, winRec->textAttributes.textPositionY, stringLengthChars);
		for (ix=0; ix < stringLengthChars; ix++) printf("%i, ", (int) textUniString[ix]);
		printf("\nPTB-ERROR: In Screen('DrawText'): Text corrupt?!?\n");
		
		free((void*)textUniString);
        goto drawtext_skipped;
    }
    CGContextSetFillColorSpace (cgContext,cgColorSpace);
    
    //check that we are in the correct alpha mode (for the debugger... should change this to exit with error)
    //correctQuartzAlphaMode= kCGImageAlphaPremultipliedLast;
    //correctQuartzAlphaModeMaybe=kCGImageAlphaPremultipliedFirst;
    //quartzAlphaMode=CGBitmapContextGetAlphaInfo(cgContext);
    
    //fill in the text background.  It's stored in the Window record in PsychColor format.  We convert it to an OpenGL color vector then into a quartz 
    // vector	
    quartzRect.origin.x=(float)0;
    quartzRect.origin.y=(float)0;
    quartzRect.size.width=(float)textureWidth;
    quartzRect.size.height=(float)textureHeight;
    PsychCoerceColorMode(&(winRec->textAttributes.textBackgroundColor));
    PsychConvertColorToDoubleVector(&(winRec->textAttributes.textBackgroundColor), winRec, backgroundColorVector);
    //by default override the background alpha value, setting alpha transparecy. Only when DrawText obeys the alpha blend function setting do we need or want the the alpha argument for the background.
    if(!PsychPrefStateGet_TextAlphaBlending())
        backgroundColorVector[3]=0;
    
    CGContextSetRGBFillColor(cgContext, (float)(backgroundColorVector[0]), (float)(backgroundColorVector[1]), (float)(backgroundColorVector[2]), (float)(backgroundColorVector[3])); 
    CGContextFillRect(cgContext, quartzRect);
    
    //now draw the text and close up the CoreGraphics shop before we proceed to textures.
    //associate the core graphics context with text layout object holding our unicode string.
    callError=ATSUSetLayoutControls (textLayout, 1, saTags, saSizes, saValue);
    ATSUDrawText(textLayout, kATSUFromTextBeginning, kATSUToTextEnd, Long2Fix((long)0), Long2Fix((long) textBoundsPRect[kPsychBottom]));
    CGContextFlush(cgContext); 	//this might not be necessary but do it just in case.
                                //free  stuff
    free((void*)textUniString);
    callError=ATSUDisposeStyle(atsuStyle);
    //TO DO: we need to get the new text location and put it back into the window record's X and Y locations.
    //Remove references from Core Graphics to the texture memory.  CG and OpenGL can share concurrently, but we don't won't need this anymore.
    CGColorSpaceRelease (cgColorSpace);
    CGContextRelease(cgContext);	
    
    // From here on: Convert the CG graphics bitmap into a GL texture.  

    // Enable this windowRecords framebuffer as current drawingtarget:
    PsychSetDrawingTarget(winRec);

	// Save all state:
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	// Disable draw shader:
	PsychSetShader(winRec, 0);

    if(!PsychPrefStateGet_TextAlphaBlending()){
        PsychGetAlphaBlendingFactorsFromWindow(winRec, &normalSourceBlendFactor, &normalDestinationBlendFactor);
        PsychStoreAlphaBlendingFactorsForWindow(winRec, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    PsychUpdateAlphaBlendingFactorLazily(winRec);
    
    // Explicitely disable Apple's Client storage extensions. For now they are not really useful to us.
    glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_FALSE);
    
    glDisable(GL_TEXTURE_RECTANGLE_EXT);
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &myTexture);				//create an index "name" for our texture
    glBindTexture(GL_TEXTURE_2D, myTexture);	//instantiate a texture of type associated with the index and set it to be the target for subsequent gl texture operators.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);		//tell gl how to unpack from our memory when creating a surface, namely don't really unpack it but use it for texture storage.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	//specify interpolation scaling rule for copying from texture.  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //specify interpolation scaling rule from copying from texture.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    PsychTestForGLErrors();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  (GLsizei)textureWidth, (GLsizei)textureHeight, 0, GL_BGRA, (bigendian) ? GL_UNSIGNED_INT_8_8_8_8_REV : GL_UNSIGNED_INT_8_8_8_8, textureMemory);
    free((void *)textureMemory);	// Free the texture memory: OpenGL has its own copy now in internal buffers.
    textureMemory = NULL;
    
    PsychTestForGLErrors();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    //The texture holding the text is >=  the bounding rect for the text because of the requirement for textures that they have sides 2^(integer)x.  
    //Therefore we texture only from the region of the texture which contains the text, not the entire texture.  Therefore the rect which we texture is the dimensions
    //of the text, not the dimensions of the texture.
    
    //texture locations
    textureTextFractionXLeft=0/textureWidth;
    textureTextFractionXRight=textWidth / textureWidth;
    textureTextFractionY= 1.0 - textHeight/textureHeight;
    
    // Final screen position of the textured text-quad:
    quadLeft=winRec->textAttributes.textPositionX;
    quadRight=winRec->textAttributes.textPositionX + textWidth;
	// quadTop needs to be adjusted by textHeightToBaseline, see above:
    quadTop=winRec->textAttributes.textPositionY - textHeightToBaseline;
    quadBottom=quadTop + textHeight;
    
    // Submit quad to pipeline:
    glBegin(GL_QUADS);
    glTexCoord2d(textureTextFractionXLeft, textureTextFractionY);		glVertex2d(quadLeft, quadTop);
    glTexCoord2d(textureTextFractionXRight, textureTextFractionY);		glVertex2d(quadRight, quadTop);
    glTexCoord2d(textureTextFractionXRight, 1.0);				glVertex2d(quadRight, quadBottom);
    glTexCoord2d(textureTextFractionXLeft, 1.0);				glVertex2d(quadLeft, quadBottom);
    glEnd();
    
    // Done with this texture:
    glDisable(GL_TEXTURE_2D);
    
    // Debug: Visualize bounding box of text on-screen:
    if (false) { 
        glColor3f(1,1,1);
        glBegin(GL_LINE_LOOP);
        glVertex2d(quadLeft, quadTop);
        glVertex2d(quadRight, quadTop);
        glVertex2d(quadRight, quadBottom);
        glVertex2d(quadLeft, quadBottom);
        glEnd();
    }
    PsychTestForGLErrors();
    
    if(!PsychPrefStateGet_TextAlphaBlending())
        PsychStoreAlphaBlendingFactorsForWindow(winRec, normalSourceBlendFactor, normalDestinationBlendFactor);
//	PsychUpdateAlphaBlendingFactorLazily(winRec);
   
    // Remove references from gl to the texture memory  & free gl's associated resources
    glDeleteTextures(1, &myTexture);	 
    
	glPopAttrib();
	
    // Update drawing cursor: Place cursor so that text could
    // be appended right-hand of the drawn text.
    winRec->textAttributes.textPositionX = quadRight;

// We jump directly to this position in the code if the textstring is empty --> No op.
drawtext_skipped:    
    // Copy out new, updated "cursor position":
    PsychCopyOutDoubleArg(1, FALSE, winRec->textAttributes.textPositionX);
    PsychCopyOutDoubleArg(2, FALSE, winRec->textAttributes.textPositionY);
    
    return(PsychError_none);
}

#else

// Implementations for Windows and Linux/X11:

#if PSYCH_SYSTEM == PSYCH_WINDOWS

// Define function prototype which is needed later:
PsychError SCREENDrawTextGDI(PsychRectType* boundingbox);

// Microsoft-Windows implementation of DrawText...
// The code below will need to be restructured and moved to the proper
// places in PTB's source tree when things have stabilized a bit...

#include <gl/glaux.h>

/* PsychOSReBuildFont
 *
 * (Re)Build a font for the specified winRec, based on OpenGL display lists.
 *
 * This routine examines the font settings for winRec and builds proper
 * OpenGL display lists that represent a font as close as possible to the
 * requested font. These routines are specific to Microsoft Windows, so they
 * need to be reimplemented for other OS'es...
 */
boolean PsychOSRebuildFont(PsychWindowRecordType *winRec)
{
  GLYPHMETRICSFLOAT	gmf[256];	// Address Buffer For Font Storage
  HFONT	font, oldfont;			// Windows Font ID
  GLuint base;
  int i;

  // Does font need to be rebuild?
  if (!winRec->textAttributes.needsRebuild) {
    // No rebuild needed. We don't have anything to do.
    return(TRUE);
  }

  // Rebuild needed. Do we have already a display list?
  if (winRec->textAttributes.DisplayList > 0) {
    // Yep. Destroy it...
    glDeleteLists(winRec->textAttributes.DisplayList, 256);
    winRec->textAttributes.DisplayList=0;
  }

  // Create Windows font object with requested properties:
  font = NULL;
  font = CreateFont(((int) (-MulDiv(winRec->textAttributes.textSize, GetDeviceCaps(winRec->targetSpecific.deviceContext, LOGPIXELSY), 72))),				// Height Of Font, aka textSize
			0,							                // Width Of Font: 0=Match to height
			0,							                // Angle Of Escapement
			0,							                // Orientation Angle
			((winRec->textAttributes.textStyle & 1) ? FW_BOLD : FW_NORMAL),		// Font Weight
			((winRec->textAttributes.textStyle & 2) ? TRUE : FALSE),		// Italic
			((winRec->textAttributes.textStyle & 4) ? TRUE : FALSE),		// Underline
			FALSE,		                // Strikeout: Set it to false until we know what it actually means...
			ANSI_CHARSET,			// Character Set Identifier: Would need to be set different for "WingDings" fonts...
			OUT_TT_PRECIS,			// Output Precision:   We try to get TrueType fonts if possible, but allow fallback to low-quality...
			CLIP_DEFAULT_PRECIS,		// Clipping Precision: Use system default.
			ANTIALIASED_QUALITY,		// Output Quality:     We want antialiased smooth looking fonts.
			FF_DONTCARE|DEFAULT_PITCH,	// Family And Pitch:   Use system default.
			winRec->textAttributes.textFontName);		// Font Name as requested by user.
  
  // Child-protection:
  if (font==NULL) {
    // Something went wrong...
    PsychErrorExitMsg(PsychError_user, "Couldn't select the requested font with the requested font settings from Windows-OS! ");
    return(FALSE);
  }

  // Select the font we created: Retain old font handle for restore below...
  oldfont=SelectObject(winRec->targetSpecific.deviceContext, font);		// Selects The Font We Created

  // Activate OpenGL context:
  PsychSetGLContext(winRec);

  // Generate 256 display lists, one for each ASCII character:
  base = glGenLists(256);
  
  // Build the display lists from the font: We want an outline font instead of a bitmapped one.
  // Characters of outline fonts are build as real OpenGL 3D objects (meshes of connected polygons)
  // with normals, texture coordinates and so on, so they can be rendered and transformed in 3D, including
  // proper texturing and lighting...
  wglUseFontOutlines(winRec->targetSpecific.deviceContext,			// Select The Current DC
		     0,								// Starting Character is ASCII char zero.
		     256,							// Number Of Display Lists To Build: 256 for all 256 chars.
		     base,							// Starting Display List handle.
		     0.0f,							// Deviation From The True Outlines: Smaller value=Smoother, but more geometry.
		     0.2f,							// Font Thickness In The Z Direction for 3D rendering.
		     ((winRec->textAttributes.textStyle & 8) ? WGL_FONT_LINES : WGL_FONT_POLYGONS),	    // Type of rendering: Filled polygons or just outlines?
		     gmf);							// Address Of Buffer To receive font metrics data.

  // Assign new display list:
  winRec->textAttributes.DisplayList = base;
  // Clear the rebuild flag:
  winRec->textAttributes.needsRebuild = FALSE;

  // Copy glyph geometry info into winRec:
  for(i=0; i<256; i++) {
    winRec->textAttributes.glyphWidth[i]=(float) gmf[i].gmfCellIncX;
    winRec->textAttributes.glyphHeight[i]=(float) gmf[i].gmfCellIncY;
  }

  // Clean up after font creation:
  SelectObject(winRec->targetSpecific.deviceContext, oldfont);		        // Restores current font selection to previous setting.
  DeleteObject(font); // Delete the now orphaned font object.

  // Our new font is ready to rock!
  return(TRUE);
}

#endif

#if PSYCH_SYSTEM == PSYCH_LINUX

// Linux/X11 implementation of PsychOSRebuildFont():

// Include of tolower() function:
#include <ctype.h>

boolean PsychOSRebuildFont(PsychWindowRecordType *winRec)
{
  char fontname[512];
  char** fontnames=NULL;
  Font font;
  XFontStruct* fontstruct=NULL;
  GLuint base;
  int i, actual_count_return;

  // Does font need to be rebuild?
  if (!winRec->textAttributes.needsRebuild) {
    // No rebuild needed. We don't have anything to do.
    return(TRUE);
  }

  // Rebuild needed. Do we have already a display list?
  if (winRec->textAttributes.DisplayList > 0) {
    // Yep. Destroy it...
    glDeleteLists(winRec->textAttributes.DisplayList, 256);
    winRec->textAttributes.DisplayList=0;
  }

  // Create X11 font object with requested properties:
  if (winRec->textAttributes.textFontName[0] == '-') {
    // Fontname supplied in X11 font name format. Just take it as is,
    // the user seems to know how to handle X11 fonts...
    snprintf(fontname, sizeof(fontname)-1, "*%s*", winRec->textAttributes.textFontName); 
  }
  else {
    // Standard Psychtoolbox font name spec: Use all the text settings that we have and
    // try to synthesize a X11 font spec string.
    snprintf(fontname, sizeof(fontname)-1, "-*-%s-%s-%s-*--%i-*-*-*", winRec->textAttributes.textFontName, ((winRec->textAttributes.textStyle & 1) ? "bold" : "regular"),
	     ((winRec->textAttributes.textStyle & 2) ? "i" : "r"), winRec->textAttributes.textSize); 
  }

  fontname[sizeof(fontname)-1]=0;
  // Convert fontname to lower-case characters:
  for(i=0; i<strlen(fontname); i++) fontname[i]=tolower(fontname[i]);

  // Try to load font:
  font = XLoadFont(winRec->targetSpecific.deviceContext, fontname);

  // Successfull?
  fontstruct = XQueryFont(winRec->targetSpecific.deviceContext, font); 

  // Child-protection:
  if (fontstruct == NULL) {
    // Something went wrong...
    printf("Failed to load X11 font with name %s.\n\n", winRec->textAttributes.textFontName);
    fontnames = XListFonts(winRec->targetSpecific.deviceContext, "*", 1000, &actual_count_return);
    if (fontnames) {
      printf("Available X11 fonts are:\n");
      for (i=0; i<actual_count_return; i++) printf("%s\n", (char*) fontnames[i]);
      printf("\n\n");
      XFreeFontNames(fontnames);
      fontnames=NULL;
    }

    printf("Failed to load X11 font with name %s.\n\n", fontname);
    PsychErrorExitMsg(PsychError_user, "Couldn't select the requested font with the requested font settings from X11 system!");
    return(FALSE);
  }

  // Activate OpenGL context:
  PsychSetGLContext(winRec);

  // Generate 256 display lists, one for each ASCII character:
  base = glGenLists(256);

  // Build the display lists from the font:
  glXUseXFont(font,
	      0,                   // Starting Character is ASCII char zero.
              256,                 // Number Of Display Lists To Build: 256 for all 256 chars.
              base                 // Starting Display List handle.
	      );
  
  // Assign new display list:
  winRec->textAttributes.DisplayList = base;

  // Clear the rebuild flag:
  winRec->textAttributes.needsRebuild = FALSE;

  // Copy glyph geometry info into winRec:
  for(i=0; i<256; i++) {
    fontname[0]=(char) i;
    fontname[1]=0;
    winRec->textAttributes.glyphWidth[i]=(float) XTextWidth(fontstruct, fontname, 1);
    winRec->textAttributes.glyphHeight[i]=(float) winRec->textAttributes.textSize;
  }

  // Release font and associated font info:
  XFreeFontInfo(NULL, fontstruct, 1);
  fontstruct=NULL;
  XUnloadFont(winRec->targetSpecific.deviceContext, font);

  // Our new font is ready to rock!
  return(TRUE);
}
#endif


// The DrawText implementation itself is identical on Windows and Linux:

// Synopsis string for Windows & Linux DrawText is different from OS-X...
static char synopsisString[] = 
    "Draw text. On MS-Windows, \"text\" may include two-byte (16 bit) Unicode characters (e.g. Chinese). "
	"A standard Matlab/Octave text string is interpreted as 8 bit ASCII string. If you "
	"want to pass a string which contains 16 bit UTF-16 unicode characters, convert the "
	"text to a double matrix, ie, mytext = double(myunicodetext); then pass the double "
	"matrix to this function. Unicode text drawing is supported if you select the default "
	"high quality, but slower GDI text renderer on Windows.\n"
	"With the optional fast, low quality renderer, neither anti-aliasing nor Unicode are "
	"supported and text positioning may be a bit less accurate, but it is a good choice if "
	"you are in need for speed over everything else. Select it via the command:\n"
	"Screen('Preference', 'TextRenderer', 0); inserted at the top of your script.\n"
	"On Linux, Unicode and anti-aliasing aren't supported yet.\n"
    "Default \"x\" \"y\" is current pen location. \"color\" is the CLUT index (scalar or [r "
    "g b] triplet) that you want to poke into each pixel; default produces black with "
    "the standard CLUT for this window's pixelSize.  \"yPositionIsBaseline\" If specified, "
	"will override the global preference setting for text positioning: It defaults to off. "
	"If it is set to 1, the y pen location defines the base line of drawn text, otherwise "
	"it defines the top of the drawn text. Old PTB's had a behaviour equivalent to setting 1, "
	"unfortunately this behaviour wasn't replicated in pre 3.0.8 PTB's so now we stick to the "
	"new behaviour by default.\n"
	"\"newX, newY\" return the final pen location. On GNU/Linux the font renderer is currently "
	"a very simple bitmap renderer which doesn't allow for Unicode rendering or anti-aliasing."
	"Btw.: Screen('Preference', ...); provides a couple of interesting text preference "
	"settings that affect text drawing, e.g., setting alpha blending and anti-aliasing modes.";

PsychError SCREENDrawText(void)
{
    PsychWindowRecordType *winRec;
    PsychRectType		     windowRect;
    char			           *textString;
    int                   stringl;
    Boolean			        doSetColor, doSetBackgroundColor;
    PsychColorType		  colorArg, backgroundColorArg;
    int				        depthValue, whiteValue, i, yPositionIsBaseline;
    float                 accumWidth, maxHeight, textHeightToBaseline, scalef; 
    static GLuint	        base=0;	     // Base Display List For The Font Set

    #if PSYCH_SYSTEM == PSYCH_WINDOWS
		 // Use GDI based text renderer on Windows, instead of display list based one?
		 if (PsychPrefStateGet_TextRenderer()==1) {
			// Call the GDI based renderer instead:
			return(SCREENDrawTextGDI(NULL));
	 	 }
	#endif
    
    // All subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    PsychErrorExit(PsychCapNumInputArgs(7));   	
    PsychErrorExit(PsychRequireNumInputArgs(2)); 	
    PsychErrorExit(PsychCapNumOutputArgs(2));  

    //Get the window structure for the onscreen window.
    PsychAllocInWindowRecordArg(1, TRUE, &winRec);
    
    //Get the dimensions of the target window
    PsychGetRectFromWindowRecord(windowRect, winRec);
    
    //Get the text string (it is required)
    PsychAllocInCharArg(2, kPsychArgRequired, &textString);
    
    //Get the X and Y positions.
    PsychCopyInDoubleArg(3, kPsychArgOptional, &(winRec->textAttributes.textPositionX));
    PsychCopyInDoubleArg(4, kPsychArgOptional, &(winRec->textAttributes.textPositionY));
    
    //Get the depth from the window, we need this to interpret the color argument.
    depthValue=PsychGetWindowDepthValueFromWindowRecord(winRec);
   
    //Get the new color record, coerce it to the correct mode, and store it.  
    doSetColor=PsychCopyInColorArg(5, kPsychArgOptional, &colorArg);
    if(doSetColor) PsychSetTextColorInWindowRecord(&colorArg,  winRec);

    // Same for background color: FIXME This is currently a no-op. Don't know yet how to
    // map this to the Windows way of font rendering...
    doSetBackgroundColor=PsychCopyInColorArg(6, kPsychArgOptional, &backgroundColorArg);
    if(doSetBackgroundColor) PsychSetTextBackgroundColorInWindowRecord(&backgroundColorArg,  winRec);

	// Special handling of offset for y position correction:
	yPositionIsBaseline = PsychPrefStateGet_TextYPositionIsBaseline();
	PsychCopyInIntegerArg(7, kPsychArgOptional, &yPositionIsBaseline);

    // Enable this windowRecords framebuffer as current drawingtarget:
    PsychSetDrawingTarget(winRec);

	// Set default draw shader on Windows, but disable on Linux, as glBitmapped rendering doesn't work with it:
	PsychSetShader(winRec, (PSYCH_SYSTEM == PSYCH_LINUX) ? 0 : -1);

	PsychUpdateAlphaBlendingFactorLazily(winRec);
	PsychCoerceColorMode( &(winRec->textAttributes.textColor));
    PsychSetGLColor(&(winRec->textAttributes.textColor), winRec);

    // Does the font (better, its display list) need to be build or rebuild, because
    // font name, size or settings have changed?
    // This routine will check it and perform all necessary ops if so...
    PsychOSRebuildFont(winRec);

    // Compute text-bounds as x and y increments:
    stringl=strlen(textString);
    accumWidth=0;
    maxHeight=0;
    for (i=0; i<stringl; i++) {
      accumWidth+=winRec->textAttributes.glyphWidth[textString[i]];
      maxHeight=(winRec->textAttributes.glyphHeight[textString[i]] > maxHeight) ? winRec->textAttributes.glyphHeight[textString[i]] : maxHeight;
    }

    accumWidth*=(PSYCH_SYSTEM == PSYCH_WINDOWS) ? winRec->textAttributes.textSize : 1.0;
    maxHeight*=(PSYCH_SYSTEM == PSYCH_WINDOWS) ? winRec->textAttributes.textSize : 1.0;

	if (yPositionIsBaseline) {
		// Y position of drawing cursor defines distance between top of text and
		// baseline of text, i.e. the textheight excluding descenders of letters:
		// FIXME: This is most likely plain wrong!!!
		textHeightToBaseline = maxHeight;
	}
	else {
		// Y position of drawing cursor defines top of text, therefore no offset (==0) needed:
		textHeightToBaseline = 0;
	}

    // Draw the text string to window by execution of the display lists in
    // proper order:

    // Backup modelview matrix:
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    #if PSYCH_SYSTEM == PSYCH_WINDOWS
    // Position our "cursor": These are 3D fonts where the glyphs are represented by 3D geometry.
    glTranslatef(winRec->textAttributes.textPositionX, winRec->textAttributes.textPositionY - textHeightToBaseline + winRec->textAttributes.textSize, -0.5f);

    // Scale to final size:
    // glScalef(winRec->textAttributes.textSize, -1 * winRec->textAttributes.textSize, 1);
	 scalef = MulDiv(winRec->textAttributes.textSize, GetDeviceCaps(winRec->targetSpecific.deviceContext, LOGPIXELSY), 72);
    glScalef(scalef, -1 * scalef, 1);
    #endif

    #if PSYCH_SYSTEM == PSYCH_LINUX
    // Position our "cursor": The X11 implementation uses glBitmap()'ed fonts, so we need to position
    // the rasterposition cursor. We need the glColor4dv() here to handle a special case when using
    // HDR framebuffers and HDR draw shaders -- this is not compatible with Linux glBitmapped() rendering...
    glColor4dv(winRec->currentColor);
    glRasterPos2f(winRec->textAttributes.textPositionX, winRec->textAttributes.textPositionY  - textHeightToBaseline + winRec->textAttributes.textSize);
    glPixelZoom(1,1);
    #endif

    // Backup display list state and state of glFrontFace(): The display lists on M$-Windows
    // contain glFrontFace() commands to change front-face order, so we need to save and
    // restore it.
    glPushAttrib(GL_LIST_BIT | GL_POLYGON_BIT);

    // Sets The Base Character to the start of our font display list:
    glListBase(winRec->textAttributes.DisplayList);

    // Render it...
    glCallLists(strlen(textString), GL_UNSIGNED_BYTE, textString);

    // Restore state:
    glPopAttrib();
    glPopMatrix();

    // Mark end of drawing op. This is needed for single buffered drawing:
    PsychFlushGL(winRec);

    // Update drawing cursor: Place cursor so that text could
    // be appended right-hand of the drawn text.
    winRec->textAttributes.textPositionX += accumWidth;

    // Copy out new, updated "cursor position":
    PsychCopyOutDoubleArg(1, FALSE, winRec->textAttributes.textPositionX);
    PsychCopyOutDoubleArg(2, FALSE, winRec->textAttributes.textPositionY);

    return(PsychError_none);
}

#if PSYCH_SYSTEM == PSYCH_WINDOWS

// New GDI based text-renderer for MS-Windows. This one is disabled by
// default, but can get enabled by the user via a Screen Preference setting,
// i.e. if 'TextRenderer' is set to 1.
//
// Reason this one is not used by default: It's sloooow. However it provides
// accurate text positioning, Unicode rendering, anti-aliasing, proper text
// size and a higher quality text output in general.
//
// It uses GDI text renderer to render text to a memory device context,
// backed by a DIB device independent memory bitmap. Then it converts the
// DIB to an OpenGL compatible RGBA format and draws it via OpenGL,
// currently via glDrawPixels, in the future maybe via texture mapping if
// that should be faster.
//
// Reasons for slowness: GDI is slow and CPU only -- no GPU acceleration,
// GDI->OpenGL data format conversion (and our trick to get an anti-aliased
// alpha-channel) is slow and compute intense, data upload and blit in GL
// is slow due to hostmemory -> VRAM copy.

// The following variables must be released at Screen flush time the latest.
// The exit routine CleanupDrawTextGDI() does this when invoked
// from the ScreenCloseAllWindows() function, as part of a Screen flush,
// error abort, or Screen('CloseAll').

// The current (last used) font for GDI text drawing:
static HFONT				font=NULL;		// Handle to current font.

// These static variables hold the memory bitmap buffers (device contexts)
// for GDI based text drawing. We keep them accross calls to DrawText, and
// only allocate them on first invocation, or reallocate them when the size
// of the target window has changed.
static HDC					dc = NULL;		// Handle to current memory device context.
static BYTE*				pBits = NULL;	// Pointer to dc's DIB bitmap memory.
static HBITMAP				hbmBuffer;		// DIB.
static HBITMAP				defaultDIB;
static int					oldWidth=-1;	// Size of last target window for drawtext.
static int					oldHeight=-1;	// dto.
static PsychWindowRecordType* oldWin = NULL; // Last window to which text was drawn to.

void CleanupDrawTextGDI(void)
{
	if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: In CleanupDrawTextGDI: Releasing GDI ressources for DrawTextGDI.\n");

	if (font) {
		if (!DeleteObject(font)) printf("PTB-WARNING: In CleanupDrawTextGDI: Failed to release font! Expect memory leaks!!!\n");
	}
	font = NULL;
	
	if (dc) {
		// Unselect hbmBuffer from dc by reselecting default DIB:
		SelectObject(dc, defaultDIB);

		// Release now detached hbmBuffer:
		if (!DeleteObject((HGDIOBJ) hbmBuffer)) printf("PTB-WARNING: In CleanupDrawTextGDI: Failed to release DIB buffer! Expect memory leaks!!!\n");

		// Delete device context:
		if (!DeleteDC(dc)) printf("PTB-WARNING: In CleanupDrawTextGDI: Failed to release device context DC! Expect memory leaks!!!\n");

		hbmBuffer = NULL;
		pBits = NULL;
		dc = NULL;
	}
	
	oldWidth = -1;
	oldHeight = -1;
	
	oldWin = NULL;
	
	return;
}

PsychError SCREENDrawTextGDI(PsychRectType* boundingbox)
{
    PsychWindowRecordType		*winRec;
    PsychRectType				windowRect, boundingRect;
    char						*textString;
	double*						unicodedoubles;
	int							stringLengthChars;
	WCHAR*						textUniString;
	int							dummy1, dummy2;
    Boolean						doSetColor, doSetBackgroundColor;
    PsychColorType				colorArg, backgroundColorArg;
    int							i, yPositionIsBaseline;
	GLdouble					incolors[4];
	unsigned char				bincolors[4];
    GLenum						normalSourceBlendFactor, normalDestinationBlendFactor;
	POINT						xy;
    int							BITMAPINFOHEADER_SIZE = sizeof(BITMAPINFOHEADER) ;
	static BITMAPINFOHEADER		abBitmapInfo;
    BITMAPINFOHEADER*			pBMIH = (BITMAPINFOHEADER*) &abBitmapInfo;
	RECT						trect, brect;
	HFONT						defaultFont;
	unsigned char				colorkeyvalue;
	unsigned char*				scanptr;
	int skiplines, renderheight;	
	DWORD outputQuality;
	textUniString = NULL;
	
	if (boundingbox == NULL) {
		// All subfunctions should have these two lines.  
		PsychPushHelp(useString, synopsisString, seeAlsoString);
		if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
		
		PsychErrorExit(PsychCapNumInputArgs(7));   	
		PsychErrorExit(PsychRequireNumInputArgs(2)); 	
		PsychErrorExit(PsychCapNumOutputArgs(2));  
	}
	
    //Get the window structure for the onscreen window.
    PsychAllocInWindowRecordArg(1, TRUE, &winRec);
    
    //Get the dimensions of the target window
    PsychGetRectFromWindowRecord(windowRect, winRec);
	
    // Standard 1 byte ASCII or UTF-16 Unicode?
	if (PsychGetArgType(2) == PsychArgType_char) {
    	// Get standard 1 byte ASCII string:
    	PsychAllocInCharArg(2, kPsychArgRequired, &textString);
		if((strlen(textString) < 1) && boundingbox) PsychErrorExitMsg(PsychError_user, "You asked me to compute the bounding box of an empty text string?!? Sorry, that's a no no...");
		if(strlen(textString) < 1) goto drawtext_skipped; // We skip most of the code if string is empty.	
		unicodedoubles = NULL;
	}
	else {
		// Not a character string: Check if it's a double matrix for Unicode text encoding:
		PsychAllocInDoubleMatArg(2, TRUE, &dummy1, &stringLengthChars, &dummy2, &unicodedoubles);
		if (dummy1!=1 || dummy2!=1) PsychErrorExitMsg(PsychError_user, "Unicode text matrices must be 1 row by character columns!");
		if((stringLengthChars < 1) && boundingbox) PsychErrorExitMsg(PsychError_user, "You asked me to compute the bounding box of an empty text string?!? Sorry, that's a no no...");
		if(stringLengthChars < 1) goto drawtext_skipped; // We skip most of the code if string is empty.		
		textUniString=(WCHAR*) malloc(sizeof(WCHAR) * stringLengthChars);
		for (dummy1=0; dummy1 < stringLengthChars; dummy1++) textUniString[dummy1] = (WCHAR) unicodedoubles[dummy1];
	}
	
    //Get the X and Y positions.
    PsychCopyInDoubleArg(3, kPsychArgOptional, &(winRec->textAttributes.textPositionX));
    PsychCopyInDoubleArg(4, kPsychArgOptional, &(winRec->textAttributes.textPositionY));
	
	// 'DrawText' mode?
	if (boundingbox == NULL) {
		// DRAWTEXT mode:
		
		//Get the new color record, coerce it to the correct mode, and store it.  
		doSetColor=PsychCopyInColorArg(5, kPsychArgOptional, &colorArg);
		if(doSetColor) PsychSetTextColorInWindowRecord(&colorArg,  winRec);
		
		// Same for background color: FIXME This is currently a no-op. Don't know yet how to
		// map this to the Windows way of font rendering...
		doSetBackgroundColor=PsychCopyInColorArg(6, kPsychArgOptional, &backgroundColorArg);
		if(doSetBackgroundColor) PsychSetTextBackgroundColorInWindowRecord(&backgroundColorArg,  winRec);
		
		// Special handling of offset for y position correction:
		yPositionIsBaseline = PsychPrefStateGet_TextYPositionIsBaseline();
		PsychCopyInIntegerArg(7, kPsychArgOptional, &yPositionIsBaseline);

		// Enable this windowRecords framebuffer as current drawingtarget:
		PsychSetDrawingTarget(winRec);
		
		PsychCoerceColorMode( &(winRec->textAttributes.textColor));
		PsychSetGLColor(&(winRec->textAttributes.textColor), winRec);
	}
	else {
		// TEXTBOUNDS mode:
		// Special handling of offset for y position correction: In 'TextBounds' mode,
		// this is the 5th argument, instead of the 7th in 'DrawText' mode:
		yPositionIsBaseline = PsychPrefStateGet_TextYPositionIsBaseline();
		PsychCopyInIntegerArg(5, kPsychArgOptional, &yPositionIsBaseline);	
	}
		
	// Reallocate device context and bitmap if needed:
	if ((dc!=NULL) && (oldWidth != PsychGetWidthFromRect(winRec->rect) || oldHeight!=PsychGetHeightFromRect(winRec->rect))) {
		// Target windows size doesn't match size of our backingstore: Reallocate...
		if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: In DrawTextGDI: Reallocating backing DC due to change in target window size: %i x %i pixels. \n", (int) PsychGetWidthFromRect(winRec->rect), (int) PsychGetHeightFromRect(winRec->rect));

		// Unselect hbmBuffer from dc by reselecting default DIB:
		SelectObject(dc, defaultDIB);

		// Release now detached hbmBuffer:
		if (!DeleteObject((HGDIOBJ) hbmBuffer)) printf("PTB-WARNING: In DrawTextGDI: Failed to release DIB buffer! Expect memory leaks!!!\n");

		// Delete device context:
		if (!DeleteDC(dc)) printf("PTB-WARNING: In DrawTextGDI: Failed to release device context DC! Expect memory leaks!!!\n");

		hbmBuffer = NULL;
		dc = NULL;		
	}
	
	// (Re-)allocation of memory device context and DIB bitmap needed?
	if (dc==NULL) {
		oldWidth=(int) PsychGetWidthFromRect(winRec->rect);
		oldHeight=(int) PsychGetHeightFromRect(winRec->rect);
		
		// Fill in the header info.
		memset(pBMIH, 0, BITMAPINFOHEADER_SIZE);
		pBMIH->biSize         = sizeof(BITMAPINFOHEADER);
		pBMIH->biWidth        = oldWidth;
		pBMIH->biHeight       = oldHeight;
		pBMIH->biPlanes       = 1;
		pBMIH->biBitCount     = 32; 
		pBMIH->biCompression  = BI_RGB; 
		
		//
		// Create the new 32-bpp DIB section.
		//
		dc = CreateCompatibleDC(NULL);
		hbmBuffer = CreateDIBSection( 		  dc,
											  (BITMAPINFO*) pBMIH,
											  DIB_RGB_COLORS,
											  (VOID **) &pBits,
											  NULL,
											  0);

		// Select DIB into DC. Store reference to default DIB:
		defaultDIB = (HBITMAP) SelectObject(dc, hbmBuffer);
	}
	
    // Does the font (better, its display list) need to be build or rebuild, because
    // font name, size or settings have changed? Or is the current window
	// winRec not identical to the last target window oldWin? In that case,
	// we'll need to reassign the font as well, as fonts are not cached
	// on a per windowRecord basis.
	//
    // This routine will check it and perform all necessary ops if so...
	if ((winRec->textAttributes.needsRebuild) || (oldWin != winRec)) {
		// Need to realloc font:
		if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: In DrawTextGDI: Rebuilding font due to window switch or rebuild request: needit = %i , oldwin = %p vs. newwin = %p \n", winRec->textAttributes.needsRebuild, oldWin, winRec);
		
		// Delete the old font object, if any:
		if (font) {
			if (!DeleteObject(font)) printf("PTB-WARNING: In DrawTextGDI: Failed to release font! Expect memory leaks!!!\n");
		}
		font = NULL; 

		switch(PsychPrefStateGet_TextAntiAliasing()) {
			case 0:		// No anti-aliasing:
				outputQuality = NONANTIALIASED_QUALITY;
				break;
			case 1:		// Anti-aliased rendering:
				outputQuality = ANTIALIASED_QUALITY;
				break;
			case 2:		// WindowsXP and later only: Cleartype anti-aliasing.
				outputQuality = 5; // This is #define CLEARTYPE_QUALITY ...
				break;
			default:	// Default to anti-aliasing:
				outputQuality = ANTIALIASED_QUALITY;
		}
		
		// Create new font object, according to new/changed specs:
		font = CreateFont(	((int) (-MulDiv(winRec->textAttributes.textSize, GetDeviceCaps(dc, LOGPIXELSY), 72))),	// Height Of Font, aka textSize
							0,							                // Width Of Font: 0=Match to height
							0,							                // Angle Of Escapement
							0,							                // Orientation Angle
							((winRec->textAttributes.textStyle & 1) ? FW_BOLD : FW_NORMAL),		// Font Weight
							((winRec->textAttributes.textStyle & 2) ? TRUE : FALSE),		// Italic
							((winRec->textAttributes.textStyle & 4) ? TRUE : FALSE),		// Underline
							FALSE,		                // Strikeout: Set it to false until we know what it actually means...
							ANSI_CHARSET,			// Character Set Identifier: Would need to be set different for "WingDings" fonts...
							OUT_TT_PRECIS,			// Output Precision:   We try to get TrueType fonts if possible, but allow fallback to low-quality...
							CLIP_DEFAULT_PRECIS,		// Clipping Precision: Use system default.
							outputQuality,		// Output Quality wrt. Anti-Aliasing.
							FF_DONTCARE|DEFAULT_PITCH,	// Family And Pitch:   Use system default.
							winRec->textAttributes.textFontName);		// Font Name as requested by user.
		
		// Child-protection:
		if (font==NULL) {
			// Something went wrong...
			PsychErrorExitMsg(PsychError_user, "Couldn't select the requested font with the requested font settings from Windows-OS! ");
		}
		
		// Clear rebuild flag:
		winRec->textAttributes.needsRebuild = FALSE;
	}
	
	// Update last target window:
	oldWin = winRec;
	
	// Select the font we created:
	defaultFont = (HFONT) SelectObject(dc, font);

	if (yPositionIsBaseline) {
		// Y position of drawing cursor defines distance between top of text and
		// baseline of text, i.e. the textheight excluding descenders of letters:
		
		// Set text alignment mode to obey and update the drawing cursor position, with the
		// y position being the text baseline:
	 	SetTextAlign(dc, TA_UPDATECP | TA_LEFT | TA_BASELINE);
	}
	else {
		// Y position of drawing cursor defines top of text:
	 	// Set text alignment mode to obey and update the drawing cursor position, with the
		// y position being the top of the text bounding box:
	 	SetTextAlign(dc, TA_UPDATECP | TA_LEFT | TA_TOP);
	}
	
	// Define targetrectangle/cliprectangle for all drawing: It is simply the full
	// target window area:
	trect.left = 0;
	trect.right = oldWidth-1;
	trect.top = 0;
	trect.bottom = oldHeight-1;
	
	// Convert PTB color into text RGBA color and set it as text color:
	PsychConvertColorToDoubleVector(&(winRec->textAttributes.textColor), winRec, incolors);
	
	// Text drawing shall be transparent where no text pixels are drawn:
	SetBkMode(dc, TRANSPARENT);
	
	// Set text color to full white:
	SetTextColor(dc, RGB(255, 255, 255));
	
	// Set drawing cursor to requested position:
	MoveToEx(dc, (int) winRec->textAttributes.textPositionX, (int) winRec->textAttributes.textPositionY, NULL);

	brect = trect;
	// printf("PRE: ltrb %d %d %d %d\n", brect.left, brect.top, brect.right, brect.bottom);

	// Pseudo-Draw the textString: Don't rasterize, just find bounding box.
	if (unicodedoubles) {
		// Drawing of Unicode text:
		DrawTextW(dc, textUniString, stringLengthChars, &brect, DT_CALCRECT);
		MoveToEx(dc, (int) winRec->textAttributes.textPositionX, (int) winRec->textAttributes.textPositionY, NULL);
	 }
	else {
		// Drawing of standard ASCII text:
		DrawText(dc, textString, -1, &brect, DT_CALCRECT);
		MoveToEx(dc, (int) winRec->textAttributes.textPositionX, (int) winRec->textAttributes.textPositionY, NULL);
	 }

	// renderheight is the total height of the rendered textbox, not taking clipping into account.
	// Its the number of pixelrows to process...
	renderheight = (int) brect.bottom - (int) brect.top;

	// Calculate skiplines - the number of pixelrows to skip from start of the DIB/from
	// bottom of targetwindow. Need to take into account, what the y position actually means:
	if (yPositionIsBaseline) {
		// y-Position is the baseline of text: Take height of "descender" area into account:
		skiplines = oldHeight - ((renderheight - winRec->textAttributes.textSize) + (int) winRec->textAttributes.textPositionY);
	}
	else {
		// y-Position is top of texts bounding box:
		skiplines = oldHeight - (renderheight + (int) winRec->textAttributes.textPositionY);
	}

	// Calculate and store bounding rectangle:
	boundingRect[kPsychTop]    = oldHeight - 1 - skiplines - renderheight;
	boundingRect[kPsychBottom] = oldHeight - 1 - skiplines;
	boundingRect[kPsychLeft]   = winRec->textAttributes.textPositionX;
	boundingRect[kPsychRight]  = winRec->textAttributes.textPositionX + (double) ((int) brect.right - (int) brect.left);

	// Is this a 'Textbounds' op?
	if (boundingbox) {
		// 'Textbounds" op, no real text drawing. Assign final bounding box, then return:
		PsychCopyRect(boundingbox, boundingRect);

		// Release unicode textstring, if any:
		if (textUniString) free(textUniString);
		
		// Restore to default font after text drawing:
		SelectObject(dc, defaultFont);

		// Done, return:
		return(PsychError_none);
	}

	// Bounds checking: Need to take text into account that is partially or fully outside
	// the windows drawing area:
	if (skiplines < 0) {
		// Lower bound of text is below lower border of window.
		// Reduce size of processing area by the difference (we add a negative value == subtract):
		renderheight = renderheight + skiplines;
		
		// Start at bottom of screen and DIB with processing:
		skiplines = 0;
	}
	
	if ((skiplines + renderheight) > (oldHeight - 1)) {
		// Upper bound of text is above upper border of window.
		// Reduce size of processing area by the difference:
		renderheight = renderheight - ((skiplines + renderheight) - (oldHeight - 1));
	}

	// Negative or zero renderheight? In that case we would be done, because the area of text
	// to really draw would be empty or less than empty!
	if (renderheight <= 0) goto drawtext_noop;
	
	// Ok, bounds checking left us with something to process and draw - Do it:
	
	// "Erase" DIB with black background color:
	scanptr = (unsigned char*) pBits + skiplines * oldWidth * 4;
	memset((void*) scanptr, 0, oldWidth * renderheight * 4);
	
	// Really draw the textString: Rasterize!
	if (unicodedoubles) {
		// Drawing of Unicode text:
		DrawTextW(dc, textUniString, stringLengthChars, &trect, DT_NOCLIP);
	 }
	else {
		// Drawing of standard ASCII text:
		DrawText(dc, textString, -1, &trect, DT_NOCLIP);
	 }

	// Sync the GDI so we have a final valid bitmap after this call:
	GdiFlush();
	
	// Loop through the bitmap: Set the unused MSB of each 32 bit DWORD to a
	// meaningful alpha-value for OpenGL.
	bincolors[0] = (unsigned int)(incolors[0] * 255);
    bincolors[1] = (unsigned int)(incolors[1] * 255);
    bincolors[2] = (unsigned int)(incolors[2] * 255);
    bincolors[3] = (unsigned int)(incolors[3] * 255);

	scanptr = (unsigned char*) pBits + skiplines * oldWidth * 4;
	for (i=0; i<oldWidth * renderheight; i++) {
		*(scanptr++) = bincolors[0];	 // Copy blue text color to blue byte.
		*(scanptr++) = bincolors[1];	 // Copy green text color to green byte.
		// Copy red byte to alpha-channel (its our anti-aliasing alpha-value), but
		// multiply with user spec'd alpha. This multiply-shift is a fast trick to
		// get normalization of the 16 bit multiply:
		colorkeyvalue = (unsigned char)((((unsigned int) *scanptr) * bincolors[3]) >> 8);
		*(scanptr++) = bincolors[2];	 // Copy red text color to red byte.
		*(scanptr++) = colorkeyvalue;	 // Copy final alpha value to alpha byte.
	}
	
	// Save all GL state:
    glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	// Setup alpha-blending for anti-aliasing, unless user script requests us to obey
	// the global blending settings set via Screen('Blendfunction') - which may be
	// suboptimal for anti-aliased text drawing:
    if(!PsychPrefStateGet_TextAlphaBlending()) {
        PsychGetAlphaBlendingFactorsFromWindow(winRec, &normalSourceBlendFactor, &normalDestinationBlendFactor);
        PsychStoreAlphaBlendingFactorsForWindow(winRec, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    PsychUpdateAlphaBlendingFactorLazily(winRec);
	
	// Enable alpha-test against an alpha-value greater zero during blit. This
	// This way, non-text pixess (with alpha equal to zero) are discarded. 
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0);

	// To conform to the OS/X behaviour, we only draw a background if user-defined alpha blending
	// is enabled:
    if(PsychPrefStateGet_TextAlphaBlending()) {
		// Draw a background color quad:
		
		// Set GL drawing color:
		PsychCoerceColorMode( &(winRec->textAttributes.textBackgroundColor));
		PsychSetGLColor(&(winRec->textAttributes.textBackgroundColor), winRec);

		// Set default draw shader:
		PsychSetShader(winRec, -1);

		// Draw background rect:
		PsychGLRect(boundingRect);
	}

    // Setup unpack mode and position for blitting of the bitmap to screen:
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// MK: Subracting one should be correct, but isn't (visually). Maybe a
	// a side-effect of gfx-rasterizer inaccuracy or off-by-one error in our
	// PsychSetupView() code?!?
	// glRasterPos2i(0,(int) oldHeight - 1 - skiplines);
	glRasterPos2i(0,(int) oldHeight - 0 - skiplines);

	// Blit it to screen: The GL_BGRA swizzles RGBA <-> BGRA properly:
	scanptr = (unsigned char*) pBits + skiplines * oldWidth * 4;

	// Disable draw shader:
	PsychSetShader(winRec, 0);

    glPixelZoom(1,1);
	glDrawPixels(oldWidth, renderheight, GL_RGBA, GL_UNSIGNED_BYTE, scanptr);
	
	// Disable alpha test after blit:
    glDisable(GL_ALPHA_TEST);
	
    // Restore state:
    if(!PsychPrefStateGet_TextAlphaBlending()) PsychStoreAlphaBlendingFactorsForWindow(winRec, normalSourceBlendFactor, normalDestinationBlendFactor);
	
	glPopAttrib();
	
    // Mark end of drawing op. This is needed for single buffered drawing:
    PsychFlushGL(winRec);

	// We jump directly to this position if text appears to be completely outside the window:
drawtext_noop:

    // Update drawing cursor: Place cursor so that text could
    // be appended right-hand of the drawn text.
    // Get updated "cursor position":
	GetCurrentPositionEx(dc, &xy);
    winRec->textAttributes.textPositionX = xy.x;
    winRec->textAttributes.textPositionY = xy.y;
	
	// Restore to default font after text drawing:
	SelectObject(dc, defaultFont);
	
	// We jump directly to this position in the code if the textstring is empty --> No op.
drawtext_skipped:    
	PsychCopyOutDoubleArg(1, FALSE, winRec->textAttributes.textPositionX);
    PsychCopyOutDoubleArg(2, FALSE, winRec->textAttributes.textPositionY);
	
	// Release unicode textstring, if any:
	if (textUniString) free(textUniString);
	
    return(PsychError_none);
}

// End of Windows specific part...
#endif

#endif
