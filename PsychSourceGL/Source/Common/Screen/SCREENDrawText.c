/*
    SCREENDrawText.c	
  
    AUTHORS:
    
		Allen.Ingling@nyu.edu		awi 
  
    PLATFORMS:
		
		Only OS X for now.
    
    HISTORY:
	
		11/17/03	awi		Spun off from SCREENTestTexture which also used Quartz and Textures to draw text but did not match the 'DrawText' specifications.
		10/12/04	awi		In useString: changed "SCREEN" to "Screen", and moved commas to inside [].
		2/25/05		awi		Added call to PsychUpdateAlphaBlendingFactorLazily().  Drawing now obeys settings by Screen('BlendFunction').
                5/08/05         mk              Bugfix for "Descenders of letters get cut/eaten away" bug introduced in PTB 1.0.5
                10/12/05        mk              Fix crash in DrawText caused by removing glFinish() while CLIENT_STORAGE is enabled!
                                                -> Disabling CLIENT_STORAGE and removing glFinish() is the proper solution...
                11/01/05        mk              Finally the real bugfix for "Descenders of letters get cut/eaten away" bug introduced in PTB 1.0.5!
                11/01/05        mk              Removal of dead code + beautification.
                11/21/05        mk              Code for updating the "Drawing Cursor" and returning NewX, NewY values added.
		01/01/06        mk              Code branch for M$-Windoze implementation of DrawText added.

    DESCRIPTION:
  
    REFERENCES:
	
		http://oss.sgi.com/projects/ogl-sample/registry/APPLE/client_storage.txt
		http://developer.apple.com/samplecode/Sample_Code/Graphics_3D/TextureRange.htm
	
  
    TO DO:
  
		- Set the alpha channel value in forground and background text correctly so that we only overwrite the portions of the target window where the text goes.
	
		- If we fail to set the font before calling this function we crash.  Fix that. 
	
		- Drawing White text works but othewise the colors don't seem to map onto the componenets correctlty.  Need to fix that.
	
		- By the way, there is something wrong with FillRect and the alpha channel does not clear the screen.
		
		- Accept 16-bit characters
	
		And remember:
	
		Destroy the shadow window after we are done with it. 
	
		Fix the color bug  See RBGA not in code below. 
		
		 
		UPDATE 3/9/05
		
		quick fix:		-Make the texture the nearest power of two which encloses the window, 
                                        -Set source alpha to 1 and target to 1-source alpha.
                                        -Use a rectangular subtexture of the full texture
		
		more complicated fix: 
		
                                        -Use size of bounding text box to determine texture size.  
                                        -(Optionally isolate rotine for bounding box to match text width of OS 9)
                                        -Obey alpha blending rules.  This makes sense when we only overwrite with a bounding box.
							
*/


#include "Screen.h"


// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.
static char useString[] = "[newX,newY]=Screen('DrawText', windowPtr, text [,x] [,y] [,color] [,backgroundColor] [,yPositionIsBaseline]);";
static char seeAlsoString[] = "";

#if PSYCH_SYSTEM == PSYCH_OSX

#define CHAR_TO_UNICODE_LENGTH_FACTOR		4			//Apple recommends 3 or 4 even though 2 makes more sense.
#define USE_ATSU_TEXT_RENDER			1

static char synopsisString[] = 
    "Draw text. \"text\" may include two-byte (16 bit) characters (e.g. Chinese). "
    "Default \"x\" \"y\" is current pen location. \"color\" is the CLUT index (scalar or [r "
    "g b] triplet) that you want to poke into each pixel; default produces black with "
    "the standard CLUT for this window's pixelSize. \"backgroundColor\" is the color of "
	"the text background. \"yPositionIsBaseline\" If specified, will override the global "
	"preference setting for text positioning: It defaults to off. If it is set to 1, the y "
	"pen location defines the base line of drawn text, otherwise it defines the top of the "
	"drawn text. Old PTB's had a behaviour equivalent to setting 1, unfortunately this behaviour "
	"wasn't replicated in pre 3.0.8 PTB's so now we stick to the new behaviour by default. "
	"\"newX, newY\" return the final pen location.";


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
	int ix;
	GLubyte* rpb;
	Boolean bigendian;
	
	// Detect endianity (byte-order) of machine:
    ix=255;
    rpb=(GLubyte*) &ix;
    bigendian = ( *rpb == 255 ) ? FALSE : TRUE;
    ix = 0; rpb = NULL;

    
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
    PsychAllocInCharArg(2, kPsychArgRequired, &textString);
    
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
    PsychAllocInCharArg(2, kPsychArgRequired, &textCString);
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
        PsychErrorExitMsg(PsychError_internal, "Failed to allocate CG Bimap Context\n");
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
    
    //Convert the CG graphics bitmap into a GL texture.  
    PsychSetGLContext(winRec);
    // Enable this windowRecords framebuffer as current drawingtarget:
    PsychSetDrawingTarget(winRec);

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//specify interpolation scaling rule for copying from texture.  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  //specify interpolation scaling rule from copying from texture.
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
  font = CreateFont(	((int) (-1 * winRec->textAttributes.textSize)),				// Height Of Font, aka textSize
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
  char fontname[256];
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
  for(i=0; i<strlen(winRec->textAttributes.textFontName); i++) fontname[i]=tolower(fontname[i]);

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
    "Draw text. \"text\" may consist of one-byte (8 bit) ASCII-Characters. "
    "Default \"x\" \"y\" is current pen location. \"color\" is the CLUT index (scalar or [r "
    "g b] triplet) that you want to poke into each pixel; default produces black with "
    "the standard CLUT for this window's pixelSize. \"newX, newY\" return the final pen "
    "location. On M$-Windows, fonts are represented as 3D geometry, so one can scale, translate "
    "and rotate them in 3D space, apply textures and lighting effects and such. On GNU/Linux "
    "the font renderer is currently a very simple bitmap renderer which doesn't allow for 3D "
    "rendering or anti-aliasing. It will get replaced in a future PTB release by something more "
    "decent. ";

PsychError SCREENDrawText(void)
{
    PsychWindowRecordType *winRec;
    PsychRectType		     windowRect;
    char			           *textString;
    int                   stringl;
    Boolean			        doSetColor, doSetBackgroundColor;
    PsychColorType		  colorArg, backgroundColorArg;
    int				        depthValue, whiteValue, i, yPositionIsBaseline;
    float                 accumWidth, maxHeight, textHeightToBaseline;

    static GLuint	        base=0;	     // Base Display List For The Font Set
    
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

    PsychSetGLContext(winRec);

    // Enable this windowRecords framebuffer as current drawingtarget:
    PsychSetDrawingTarget(winRec);

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
    glScalef(winRec->textAttributes.textSize, -1 * winRec->textAttributes.textSize, 1);
    #endif

    #if PSYCH_SYSTEM == PSYCH_LINUX
    // Position our "cursor": The X11 implementation uses glBitmap()'ed fonts, so we need to position
    // the rasterposition cursor...
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

#endif

