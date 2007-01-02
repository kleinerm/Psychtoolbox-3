/*
	SCREENTextBounds.c	
  
    AUTHORS:
    
		Allen.Ingling@nyu.edu		     awi 
		Mario.Kleiner@tuebingen.mpg.de  mk
  
    PLATFORMS:	
	
		Contains separate implementations for OS-X and M$-Windows.
    

    HISTORY:
    
		1/19/04		awi		Wrote it.
		11/1/05     mk       Resynced implementation with changes in SCREENDrawText.
		1/29/06     mk       Implemented Windows-Version of it.
		3/7/06		awi		Changed references from "Font" to "FontInfo".  The function had been previously renamed, but not all references updated.   
	
    DESCRIPTION:
  
    NOTES:
	
		According to Apple's ATSU reference:
		"... use the Font Manager functions that translate font family numbers to FMFont values, which are equivalent to ATSUFontID values.
		Font family numbers were used by QuickDraw to represent fonts to the Font Manager. Some of these fonts, even if compatible with ATSUI, may not have font IDs"
		
		This is confusing because font families are not equivalent to fonts.  FMFont values describe fonts and not font families.  A font is a member of a font family.
		They must be talking about 
		
		OSStatus FMGetFontFromFontFamilyInstance (
			FMFontFamily iFontFamily, 
			FMFontStyle iStyle, 
			FMFont *oFont, 
			FMFontStyle *oIntrinsicStyle
		);
	
  
    TO DO:
	
		¥ Associating a quartz surface with the ATSU renderer gives different results becuase it changes the number of antialiasing bits.  Therefore we should do that
		before calculating the bounds of the text, but its probably not crucial.
		
		¥ This should be spun off into a stand-alone C function so we can use it from other Screen module subfunctions.
		
		¥ The sections which convert a C string to an unicode string should be spun off into function.
		
		¥ The common parts of DrawText and TextBounds should be spun off into "PsychGetATSULayoutFromWindowAndText" or something.
  

*/


#include "Screen.h"

#define CHAR_TO_UNICODE_LENGTH_FACTOR		4							//Apple recommends 3 or 4 even though 2 makes more sense.
//For a list of text encodings see:
//http://developer.apple.com/documentation/Carbon/Reference/Text_Encodin_sion_Manager/index.html?http://developer.apple.com/documentation/Carbon/Reference/Text_Encodin_sion_Manager/tec_refchap/data_type_5.html#//apple_ref/doc/c_ref/ConstUniCharArrayPtr 
//If we can choose the endoding based on window's selected font that would be better than that would be better than always using MacRoman.  


// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "[normBoundsRect, offsetBoundsRect]= Screen('TextBounds', windowPtr, text [,x] [,y] [,yPositionIsBaseline]);";
//                          1               2                                      1          2     3     4    5    
static char synopsisString[] = 
    "Accept a window pointer and a string.  Return in normBoundsRect a rect defining the size of the text "
    "in units of pixels.  Return in offsetBoundsRect offsets of the text bounds from the origin, assuming "
    "that the text will be drawn at the current position of the text drawing cursor. Currently, only the "
    "OS-X version returns a perfect bounding box. The M$-Windows and GNU/Linux versions return a bounding "
    "box which doesn't take letters with descenders into account - Descenders are outside the returned box. ";
static char seeAlsoString[] = "";

#if PSYCH_SYSTEM == PSYCH_OSX

PsychError SCREENTextBounds(void) 
{
	//for debugging
	TextEncodingBase		textEncodingBase;
	TextEncodingVariant		textEncodingVariant;
	TextEncodingFormat		textEncodingFormat;
	
	///////
        PsychWindowRecordType           *winRec;
	char				*textCString;
	Str255				textPString;
	UniChar				*textUniString;
	OSStatus			callError;
	PsychRectType			resultPsychRect, resultPsychNormRect;
	ATSUTextLayout			textLayout;				//layout is a pointer to an opaque struct.
	int				stringLengthChars;
	int				uniCharBufferLengthElements, uniCharBufferLengthChars, uniCharBufferLengthBytes, yPositionIsBaseline;
	double			textHeightToBaseline;
	ByteCount			uniCharStringLengthBytes;
	TextToUnicodeInfo		textToUnicodeInfo;
	TextEncoding			textEncoding;
	ATSUStyle			atsuStyle;
	Boolean				foundFont;
	
	//for ATSU  style attributes
	PsychFontStructPtrType  psychFontRecord;

    			
        //all subfunctions should have these two lines.  
        PsychPushHelp(useString, synopsisString, seeAlsoString);
        if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
        //check for correct the number of arguments before getting involved
        PsychErrorExit(PsychCapNumInputArgs(5));   	
        PsychErrorExit(PsychRequireNumInputArgs(2)); 	
        PsychErrorExit(PsychCapNumOutputArgs(2));
	
	//get the window pointer and the text string and check that the window record has a font set
        PsychAllocInWindowRecordArg(1, kPsychArgRequired, &winRec);
	foundFont=PsychGetFontRecordFromFontNumber(winRec->textAttributes.textFontNumber, &psychFontRecord);
	if(!foundFont)
		PsychErrorExitMsg(PsychError_user, "Attempt to determine the bounds of text with no font or invalid font number");
		//it would be better to both prevent the user from setting invalid font numbers and init to the OS 9  default font.
	
	//read in the string and get its length and convert it to a unicode string.
	PsychAllocInCharArg(2, kPsychArgRequired, &textCString);
	stringLengthChars=strlen(textCString);
	if(stringLengthChars < 1) PsychErrorExitMsg(PsychError_user, "You asked me to compute the bounding box of an empty text string?!? Sorry, that's a no no...");
	if(stringLengthChars > 255) PsychErrorExitMsg(PsychError_unimplemented, "Cut corners and TextBounds will not accept a string longer than 255 characters");
	CopyCStringToPascal(textCString, textPString);
	uniCharBufferLengthChars= stringLengthChars * CHAR_TO_UNICODE_LENGTH_FACTOR;
	uniCharBufferLengthElements= uniCharBufferLengthChars + 1;		
	uniCharBufferLengthBytes= sizeof(UniChar) * uniCharBufferLengthElements;
	textUniString=(UniChar*)malloc(uniCharBufferLengthBytes);

    PsychCopyInDoubleArg(3, kPsychArgOptional, &(winRec->textAttributes.textPositionX));
    PsychCopyInDoubleArg(4, kPsychArgOptional, &(winRec->textAttributes.textPositionY));

	//Using a TextEncoding type describe the encoding of the text to be converteed.  
	textEncoding=CreateTextEncoding(kTextEncodingMacRoman, kMacRomanDefaultVariant, kTextEncodingDefaultFormat);
	//Take apart the encoding we just made to check it:
        textEncodingBase=GetTextEncodingBase(textEncoding);
        textEncodingVariant=GetTextEncodingVariant(textEncoding);
        textEncodingFormat=GetTextEncodingFormat(textEncoding);
	//Create a structure holding conversion information from the text encoding type we just created.
	callError=CreateTextToUnicodeInfoByEncoding(textEncoding,&textToUnicodeInfo);
	//Convert the text to a unicode string
	callError=ConvertFromPStringToUnicode(textToUnicodeInfo, textPString, (ByteCount)uniCharBufferLengthBytes,	&uniCharStringLengthBytes,	textUniString);
	//create the text layout object
	callError=ATSUCreateTextLayout(&textLayout);			
	//associate our unicode text string with the text layout object
	callError=ATSUSetTextPointerLocation(textLayout, textUniString, kATSUFromTextBeginning, kATSUToTextEnd, (UniCharCount)stringLengthChars);
	
	//create an ATSU style object
	callError=ATSUCreateStyle(&atsuStyle);
	callError=ATSUClearStyle(atsuStyle);
	
	//Not that we have a style object we have to set style charactersitics.  These include but are more general than Font Manager styles.  
	//ATSU Style objects have three sets of characteristics:  attributes, variations, and features.
	//attributes are things we need to set to match OS 9 behavior, such as the font ID, size, boldness, and italicization.
	//features are esoteric settings which we don't need for reproducing OS 9 behavior.  Whatever clearstyle sets should be fine.
	//font variations are axes of variation through the space of font characteristics.  The font definition includes available axes of variation.  Something else we can ignore for now.  
	PsychSetATSUStyleAttributesFromPsychWindowRecord(atsuStyle, winRec);
	//don't bother to set the variations of the style.
	//don't bother to set the features of the style.
	
	//associate the style with our layout object. This call assigns a style to every character of the string to be displayed.  
	callError=ATSUSetRunStyle(textLayout, atsuStyle, (UniCharArrayOffset)0, (UniCharCount)stringLengthChars);

	// Define the meaning of the y position of the specified drawing cursor.
	// We get the global setting from the Screen preference, but allow to override
	// it on a per-invocation basis via the optional 7th argument to 'DrawText':
	yPositionIsBaseline = PsychPrefStateGet_TextYPositionIsBaseline();
	PsychCopyInIntegerArg(5, kPsychArgOptional, &yPositionIsBaseline);
	 
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
	
	resultPsychRect[kPsychLeft]=(Fix2X(trapezoid.upperLeft.x) < Fix2X(trapezoid.lowerLeft.x)) ? Fix2X(trapezoid.upperLeft.x) : Fix2X(trapezoid.lowerLeft.x);
	resultPsychRect[kPsychRight]=(Fix2X(trapezoid.upperRight.x) > Fix2X(trapezoid.lowerRight.x)) ? Fix2X(trapezoid.upperRight.x) : Fix2X(trapezoid.lowerRight.x);
	resultPsychRect[kPsychTop]=(Fix2X(trapezoid.upperLeft.y) < Fix2X(trapezoid.upperRight.y)) ? Fix2X(trapezoid.upperLeft.y) : Fix2X(trapezoid.upperRight.y);
	resultPsychRect[kPsychBottom]=(Fix2X(trapezoid.lowerLeft.y) > Fix2X(trapezoid.lowerRight.y)) ? Fix2X(trapezoid.lowerLeft.y) : Fix2X(trapezoid.lowerRight.y);
	
	PsychNormalizeRect(resultPsychRect, resultPsychNormRect);
	resultPsychRect[kPsychLeft]=resultPsychNormRect[kPsychLeft] + winRec->textAttributes.textPositionX;
	resultPsychRect[kPsychRight]=resultPsychNormRect[kPsychRight] + winRec->textAttributes.textPositionX;
	resultPsychRect[kPsychTop]=resultPsychNormRect[kPsychTop] + winRec->textAttributes.textPositionY - textHeightToBaseline;
	resultPsychRect[kPsychBottom]=resultPsychNormRect[kPsychBottom] + winRec->textAttributes.textPositionY - textHeightToBaseline;

	PsychCopyOutRectArg(1, FALSE, resultPsychNormRect);
	PsychCopyOutRectArg(2, FALSE, resultPsychRect);


	//release resources
	free((void*)textUniString);
	callError=ATSUDisposeStyle(atsuStyle);

    return(PsychError_none);
}


	
/*	
typedef struct {
        //when adding new fields remember to initialize them to sane values within PsychInitTextRecordSettings() which is called with
        //	each new window.  
        PsychTextDrawingModeType	textMode;
        double						textPositionX;  
        double						textPositionY; 	
        int							textSize;
        int							textStyle;  // 0=normal,1=bold,2=italic,4=underline,8=outline,32=condense,64=extend		
        Str255						textFontName;
        int							textFontNumber;
} PsychTextAttributes;

typedef struct _PsychFontStructType_{

        //list management
        int					fontNumber;
        PsychFontStructPtrType			next;
 
        //Names associated with the font.  We could also add the ATS
        Str255					fontFMName;
        Str255					fontFMFamilyName;
        Str255					fontFamilyQuickDrawName;
        Str255					fontPostScriptName;
        char					fontFile[kPsychMaxFontFileNameChars];  	
			
        //depricated font style stuff for compatability with OS 9 scripts
        FMFontStyle				fontFMStyle;
        int						fontFMNumStyles;
        
        //Retain the the Font Manager (FM) and Apple Type Services (ATS) references to the font.
        //We could use these to tie font families to font names, for example 
        // FontInfo('GetFontsFromFontFamilyName') or FontInfo('GetFontFamilyFromFont');
        ATSFontRef					fontATSRef;
        FMFont						fontFMRef;
        ATSFontFamilyRef			fontFamilyATSRef;
        FMFontFamily				fontFamilyFMRef;
        
        //font's language and country
        PsychFontLocaleStructType		locale;			
        
        //font metrics
        PsychFontMetricsStructType		horizontalMetrics;
        PsychFontMetricsStructType		verticalMetrics;
} PsychFontStructType;

*/



/*
	PsychSetATSUTStyleAttributesFromPsychFontRecord()
	
	Use the Font Manager attributes for the given window, which are stored in the PsychFontAtributeStruct within the window's record, to set the 
	attributes of the ATSU style.
	
	We have to translate between Font Manager attributes and ATSU attributes ourselves, because OS X will not do it for us.
*/
/*
PsychSetATSUTStyleAttributesFromPsychAttributes(ATSUStyle style, PsychTextAttributes	psychTextAttributes)
{
	Boolean					foundFont;
	PsychFontStructType		*fontRecord;
	//for ATSU attributes
	ATSUFontID				atsuFontID;
	Fixed					atsuSize;
	Boolean					isBold;
	Boolean					isItalic;
	Boolean					isUnderline;
	Boolean					isCondensed;
	Boolean					isExtended;
	ATSUAttributeTag		aaTags[] =  {kATSUFontTag, kATSUSizeTag, kATSUQDBoldTag, kATSUItalicTag, kATSUUnderlineTag, kATSUCondensedTag, kATSUExtendedTag};
	ByteCount				aaSizes[] = {sizeof(ATSUFontID) sizeof(Fixed), sizeof(Boolean), sizeof(Boolean), sizeof(Boolean), sizeof(Boolean), sizeof(Boolean) };
	ATSUAttributeValuePtr   aaValue[] = {&atsuFontID, &atsuSize, &isBold, &isItalic, &isUnderline, &isCondensed, &isExtended};
	
	//get the font record for font number given within the window record's text attributes substruct
	foundFont=PsychGetFontRecordFromFontNumber(psychTextAttributes->textFontNumber, PsychFontStructType &fontRecord);
	
}
*/

#else

// M$-Windows and Linux/X11 version of TextBounds:

// This function PsychOSRebuildFont() is implemented in the Windows-portion
// of SCREENDrawText.c
boolean PsychOSRebuildFont(PsychWindowRecordType *winRec);

PsychError SCREENTextBounds(void)
{
   PsychWindowRecordType  *winRec;
   char			           *textString;
   int                    stringl, i;
	PsychRectType			  resultPsychRect, resultPsychNormRect;
   float                  accumWidth, maxHeight;

    // All subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    PsychErrorExit(PsychCapNumInputArgs(5));   	
    PsychErrorExit(PsychRequireNumInputArgs(2)); 	
    PsychErrorExit(PsychCapNumOutputArgs(2));  

    //Get the window structure for the onscreen window.
    PsychAllocInWindowRecordArg(1, TRUE, &winRec);
    
    //Get the text string (it is required)
    PsychAllocInCharArg(2, kPsychArgRequired, &textString);

    PsychCopyInDoubleArg(3, kPsychArgOptional, &(winRec->textAttributes.textPositionX));
    PsychCopyInDoubleArg(4, kPsychArgOptional, &(winRec->textAttributes.textPositionY));

	 // Enable GL context of this window - we might need it:
    PsychSetGLContext(winRec);

    // Does the font (== it's display list) need to be build or rebuild, because
    // font name, size or settings have changed?
    // This routine will check it and perform all necessary ops if so...
    PsychOSRebuildFont(winRec);

    // Top-Left bounds of text are current (x,y) position of text drawing cursor:
    resultPsychRect[kPsychLeft] = winRec->textAttributes.textPositionX;
    resultPsychRect[kPsychTop]  = winRec->textAttributes.textPositionY;

    // Compute text x and y increments:
    stringl=strlen(textString);
    accumWidth=0;
    maxHeight=0;
    for (i=0; i<stringl; i++) {
      accumWidth+=winRec->textAttributes.glyphWidth[textString[i]];
      maxHeight=(fabs(winRec->textAttributes.glyphHeight[textString[i]]) > maxHeight) ? fabs(winRec->textAttributes.glyphHeight[textString[i]]) : maxHeight;
    }

    accumWidth*=(PSYCH_SYSTEM == PSYCH_WINDOWS) ? winRec->textAttributes.textSize : 1.0;
    maxHeight*=(PSYCH_SYSTEM == PSYCH_WINDOWS) ? winRec->textAttributes.textSize : 1.0;

    resultPsychRect[kPsychRight]  = winRec->textAttributes.textPositionX + accumWidth;

    // MK: This should work according to spec, but f%$!*g Windows only returns zero values for
	 // for glyphHeight, so maxHeight is always zero :(
    // resultPsychRect[kPsychBottom] = winRec->textAttributes.textPositionY + maxHeight;

    // As fallback, we use this: It gives correct Bottom-Bound for character strings with characters that
    // don't contain descenders. The extra height of characters with descenders is not taken into account.
    resultPsychRect[kPsychBottom] = winRec->textAttributes.textPositionY + winRec->textAttributes.textSize;


    // Compute normalized version which just encodes text bounding box, not text position box:
	 PsychNormalizeRect(resultPsychRect, resultPsychNormRect);

    // Return optional values:
	 PsychCopyOutRectArg(1, FALSE, resultPsychNormRect);
	 PsychCopyOutRectArg(2, FALSE, resultPsychRect);

    // Done.
    return(PsychError_none);
}

#endif

