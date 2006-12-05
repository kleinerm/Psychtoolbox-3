/*
	PsychToolbox2/Source/OSX/Fonts/PsychFontGlue.c
	
	PLATFORMS:	This is the OS X version.  
				
	AUTHORS:
	Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	11/24/03		awi		Wrote it 
							
	DESCRIPTION:
        

*/
#include "PsychFontGlue.h"

#define 	PsychFMIterationScope			kFMLocalIterationScope
#define 	kPsychMaxFontFileNameChars		1024

//fixes discrepancy between Apple documentation & Apple ATS.h header file.
#define kATSOptionFlagsUnrestrictedScope		kATSOptionFlagsUnRestrictedScope	

/*
Note: When you iterate using a local context and an unrestricted scope, you
enumerate the default font families. This includes all globally activated font
families and those activated locally to your application.  In other words, its
what's available to your app.  
*/
#define PSYCH_ATS_ITERATOR_CONTEXT			kATSFontContextLocal
#define PSYCH_ATS_ITERATOR_SCOPE			kATSOptionFlagsUnrestrictedScope				

//declare file local functions
static void						PsychInitFontList(void);
static PsychFontStructPtrType	PsychFontListHeadKeeper(Boolean set, PsychFontStructPtrType value);




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Functions for maintaining and accessing psych font lists.

/*
    PsychFontListHeadKeeper()
    
    A file-local function which enforces limited access to the font list head by encapsulating it within a function.
    
    Only PsychGetFontListHead(), PsychInitFontList() and PsychFreeFontList should call PsychFontListHeadKeeper().
    
    Other functions needing access to the font list should get the list head only through 
    PsychGetFontListHead().
    
*/
static PsychFontStructPtrType	PsychFontListHeadKeeper(Boolean set, PsychFontStructPtrType value)
{
    static PsychFontStructPtrType		fontListHead=NULL;

    if(set){
        if(value && fontListHead)
            PsychErrorExitMsg(PsychError_internal, "Attempt to set new font list head when one is already set."); 
        fontListHead=value;
        return(fontListHead);
    }else
        return(fontListHead);
}



/*
    GetFontListHead(void)
    
    Get the list head of the font list which we built.  To insure that the font list has been initialized before
    it is accessed  We go through GetFontListHead() instead of allowing direct access to the font list variable.   
*/
PsychFontStructPtrType	PsychGetFontListHead(void)
{
    PsychFontStructPtrType	fontListHead;
    
    fontListHead=PsychFontListHeadKeeper(FALSE, NULL);
    if(!fontListHead){
        PsychInitFontList();
        fontListHead=PsychFontListHeadKeeper(FALSE, NULL);
    }
    return(fontListHead);
}





/*
    PsychFreeFontList()
    
    Free memory used to hold the Psychtoolbox's font list.  Set the list head to NULL when we
    are done to flag an empty list. 
*/
int PsychFreeFontList(void)
{
    PsychFontStructPtrType	current, previous;
    int				numFreed;
    
    numFreed=0;
    previous=NULL;
    current=PsychFontListHeadKeeper(FALSE, NULL); //get the font list head.
    while(current){
        if(previous){		
            free(previous);
            ++numFreed;
        }
        previous=current;
        current=current->next;
    }
    if(previous){		
        free(previous);			
        ++numFreed;
    }
    PsychFontListHeadKeeper(TRUE, NULL); //set the font list head to null to mark an empty list.
        
    return(numFreed);
}


/*
    PsychInitFontList()
    
    Build a list of records describing installed fonts.
    
    
*/
void PsychInitFontList(void)
{
    ATSFontRef			tempATSFontRef;
    //for font structures
    PsychFontStructPtrType	fontListHead, fontRecord, previousFontRecord;
    //for ATI font iteration
    ATSFontIterator		fontIterator;
    OSStatus			halt;
    //for font field names
    CFStringRef 		cfFontName;
    int				i;
    Boolean			resultOK;
    //for font file
    //OSErr			osError;
    OSStatus			osStatus;			
    FSSpec			fontFileSpec;
    FSRef			fontFileRef;
    //for the font metrics
    ATSFontMetrics		horizontalMetrics;
    ATSFontMetrics		verticalMetrics;
    //for info from Font Manager 
    FMFontStyle			fmStyle;
    OSStatus			fmStatus;
    Str255				fmFontFamilyNamePString;
    //whatever
    Str255			fontFamilyQuickDrawNamePString;
    TextEncoding		textEncoding;
    OSStatus			scriptInfoOK;
    ScriptCode			scriptCode;
    LangCode			languageCode;
    OSStatus			localOK;
    LocaleRef			locale;


    fontListHead=PsychFontListHeadKeeper(FALSE, NULL); //get the font list head.
    if(fontListHead)
        PsychErrorExitMsg(PsychError_internal, "Attempt to set new font list head when one is already set.");
        
    fontRecord=NULL;
    halt= ATSFontIteratorCreate(PSYCH_ATS_ITERATOR_CONTEXT, NULL, NULL, PSYCH_ATS_ITERATOR_SCOPE, &fontIterator);
    i=0;
    while(halt==noErr){
        halt=ATSFontIteratorNext(fontIterator, &tempATSFontRef);
        if(halt==noErr){
            //create a new  font  font structure.  Set the next field  to NULL as  soon as we allocate the font so that if 
            //we break with an error then we can find the end when we  walk down the linked list. 
            fontRecord=(PsychFontStructPtrType)malloc(sizeof(PsychFontStructType));
            fontRecord->next=NULL;
            if(i==0)
                PsychFontListHeadKeeper(TRUE, fontRecord); 
            else
                previousFontRecord->next=fontRecord;
            //set the font number field of the struct
            fontRecord->fontNumber=i+1;
            //Get  FM and ATS font and font family references from the ATS font reference, which we get from iteration.
            fontRecord->fontATSRef=tempATSFontRef; 
            fontRecord->fontFMRef=FMGetFontFromATSFontRef(fontRecord->fontATSRef);
            fmStatus=FMGetFontFamilyInstanceFromFont(fontRecord->fontFMRef, &(fontRecord->fontFamilyFMRef), &fmStyle);
            fontRecord->fontFamilyATSRef=FMGetATSFontFamilyRefFromFontFamily(fontRecord->fontFamilyFMRef);
            //get the font name and set the the corresponding field of the struct
            if (ATSFontGetName(fontRecord->fontATSRef, kATSOptionFlagsDefault, &cfFontName)!=noErr) {
                PsychFreeFontList();
                PsychErrorExitMsg(PsychError_internal, "Failed to query font name in ATSFontGetName! OS-X font handling screwed up?!?");
            }
            
            resultOK=CFStringGetCString(cfFontName, fontRecord->fontFMName, 255, kCFStringEncodingASCII);
            if(!resultOK){
                CFRelease(cfFontName);
                PsychFreeFontList();
                PsychErrorExitMsg(PsychError_internal, "Failed to convert CF string to char string.");
            }
            CFRelease(cfFontName);
            //get the font postscript name and set the corresponding field of the struct
            if (ATSFontGetPostScriptName(fontRecord->fontATSRef, kATSOptionFlagsDefault, &cfFontName)!=noErr) {
                PsychFreeFontList();
                mexPrintf("The following font makes trouble: %s. Please remove the offending font file from your font folders.", fontRecord->fontFMName);
                PsychErrorExitMsg(PsychError_system, "ATSFontGetPostscriptName failed.");
            }
            resultOK=CFStringGetCString(cfFontName, fontRecord->fontPostScriptName, 255, kCFStringEncodingASCII); //kCFStringEncodingASCII matches MATLAB for 0-127
            if(!resultOK){
                CFRelease(cfFontName);
                PsychFreeFontList();
                PsychErrorExitMsg(PsychError_internal, "Failed to convert CF string to char string.");
            }
            CFRelease(cfFontName);
            //get the QuickDraw name of the font
            ATSFontFamilyGetQuickDrawName(fontRecord->fontFamilyATSRef, fontFamilyQuickDrawNamePString);
            CopyPascalStringToC(fontFamilyQuickDrawNamePString, fontRecord->fontFamilyQuickDrawName);

            //get the font file used for this font
            osStatus= ATSFontGetFileSpecification(fontRecord->fontATSRef, &fontFileSpec);
            if(osStatus != noErr)
                PsychErrorExitMsg(PsychError_internal, "Failed to get the font file specifier.");
            FSpMakeFSRef(&fontFileSpec, &fontFileRef);
            osStatus= FSRefMakePath(&fontFileRef, fontRecord->fontFile, (UInt32)(kPsychMaxFontFileNameChars - 1));
            if(osStatus!=noErr){
                PsychFreeFontList();
                PsychErrorExitMsg(PsychError_internal, "Failed to get the font file path.");
            }
            //get the font metrics of this font.
            //Explicit copy between fields to make it clear what is going one, will likely have to mix & match between native and Psych structures for different
            //platforms.
            ATSFontGetHorizontalMetrics(fontRecord->fontATSRef, kATSOptionFlagsDefault, &horizontalMetrics);
            ATSFontGetVerticalMetrics(fontRecord->fontATSRef, kATSOptionFlagsDefault, &verticalMetrics);
            //horizontal
            fontRecord->horizontalMetrics.ascent=			horizontalMetrics.ascent;
            fontRecord->horizontalMetrics.descent=			horizontalMetrics.descent;
            fontRecord->horizontalMetrics.leading=			horizontalMetrics.leading;
            fontRecord->horizontalMetrics.avgAdvanceWidth=		horizontalMetrics.avgAdvanceWidth;
            fontRecord->horizontalMetrics.minLeftSideBearing=		horizontalMetrics.minLeftSideBearing;
            fontRecord->horizontalMetrics.minRightSideBearing=		horizontalMetrics.minRightSideBearing;
            fontRecord->horizontalMetrics.stemWidth=			horizontalMetrics.stemWidth;
            fontRecord->horizontalMetrics.stemHeight=			horizontalMetrics.stemHeight;
            fontRecord->horizontalMetrics.capHeight=			horizontalMetrics.capHeight;
            fontRecord->horizontalMetrics.xHeight=			horizontalMetrics.xHeight;
            fontRecord->horizontalMetrics.italicAngle=			horizontalMetrics.italicAngle;
            fontRecord->horizontalMetrics.underlinePosition=		horizontalMetrics.underlinePosition;
            fontRecord->horizontalMetrics.underlineThickness=		horizontalMetrics.underlineThickness;
            fontRecord->horizontalMetrics.underlineThickness=		horizontalMetrics.underlineThickness;
            //vertical
            fontRecord->verticalMetrics.ascent=				verticalMetrics.ascent;
            fontRecord->verticalMetrics.descent=			verticalMetrics.descent;
            fontRecord->verticalMetrics.leading=			verticalMetrics.leading;
            fontRecord->verticalMetrics.avgAdvanceWidth=		verticalMetrics.avgAdvanceWidth;
            fontRecord->verticalMetrics.minLeftSideBearing=		verticalMetrics.minLeftSideBearing;
            fontRecord->verticalMetrics.minRightSideBearing=		verticalMetrics.minRightSideBearing;
            fontRecord->verticalMetrics.stemWidth=			verticalMetrics.stemWidth;
            fontRecord->verticalMetrics.stemHeight=			verticalMetrics.stemHeight;
            fontRecord->verticalMetrics.capHeight=			verticalMetrics.capHeight;
            fontRecord->verticalMetrics.xHeight=			verticalMetrics.xHeight;
            fontRecord->verticalMetrics.italicAngle=			verticalMetrics.italicAngle;
            fontRecord->verticalMetrics.underlinePosition=		verticalMetrics.underlinePosition;
            fontRecord->verticalMetrics.underlineThickness=		verticalMetrics.underlineThickness;
            fontRecord->verticalMetrics.underlineThickness=		verticalMetrics.underlineThickness;
            //use Font Manager to get the FM  font family name font style
            fmStatus=FMGetFontFamilyName(fontRecord->fontFamilyFMRef, fmFontFamilyNamePString);
            CopyPascalStringToC(fmFontFamilyNamePString, fontRecord->fontFMFamilyName);
            fontRecord->fontFMStyle=fmStyle;
            fontRecord->fontFMNumStyles=PsychFindNumFMFontStylesFromStyle(fmStyle);
            fontRecord->fontFMNumStyles= fontRecord->fontFMNumStyles ? fontRecord->fontFMNumStyles : 1; //because the name is "normal" even if there are no styles.  
            //get the locale info which is a property of the font family
            textEncoding=ATSFontFamilyGetEncoding(fontRecord->fontFamilyATSRef);
            scriptInfoOK=RevertTextEncodingToScriptInfo(textEncoding, &scriptCode, &languageCode, NULL);
            localOK=LocaleRefFromLangOrRegionCode(languageCode, kTextRegionDontCare, &locale); 
            localOK=LocaleRefGetPartString(locale, kLocaleLanguageMask, 255, fontRecord->locale.language);			fontRecord->locale.language[255]='\0'; 
            localOK=LocaleRefGetPartString(locale, kLocaleLanguageVariantMask, 255, fontRecord->locale.languageVariant);	fontRecord->locale.languageVariant[255]='\0';
            localOK=LocaleRefGetPartString(locale, kLocaleRegionMask, 255, fontRecord->locale.region);			fontRecord->locale.region[255]='\0';
            localOK=LocaleRefGetPartString(locale, kLocaleRegionVariantMask, 255, fontRecord->locale.regionVariant);	fontRecord->locale.regionVariant[255]='\0';
            localOK=LocaleRefGetPartString(locale, kLocaleAllPartsMask, 255, fontRecord->locale.fullName);		fontRecord->locale.fullName[255]='\0';
            //increment the font index and update the next font pointer
            ++i;
            previousFontRecord=fontRecord;
        }else if(halt == kATSIterationScopeModified){
            //exit because the font database changed during this loop.
            PsychFreeFontList();
            PsychErrorExitMsg(PsychError_internal, "The system font database was modified during the function call");
        }
    }
    if(halt != kATSIterationCompleted){
        PsychFreeFontList();
        PsychErrorExitMsg(PsychError_internal, "Font iteration terminated prematurely");
    }
    ATSFontIteratorRelease(&fontIterator);
    
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Functions which use font lists.


/*
    PsychGetFontListLength()
    
    Walk down the list and count the number of elements in the list
*/
int PsychGetFontListLength(void)
{
    PsychFontStructPtrType		current;
    int 				i;
    
    i=0;
    for(current=PsychGetFontListHead();  current; current=current->next)
        ++i;
    return(i);
}




/*
    PsychGetFontRecordFromFontNumber()
    
    Accept a font number.  Lookup the specified font record in the list of font records.  If the font is found in the 
    list then set the fontStruct pointer to point to the font record.  Otherwise set fontStruct to NULL and return FALSE.   
        
    Font numbers are something the Psychtoolbox assigns when building its font list.  They are not genine properties of the 
    font.
*/
Boolean	PsychGetFontRecordFromFontNumber(int fontIndex, PsychFontStructType **fontStruct)
{
    PsychFontStructPtrType	current;
    Boolean			found;

    found=0;
    for(current=PsychGetFontListHead();  current; current=current->next){
        if(current->fontNumber==fontIndex){
            found=1;
            *fontStruct=current;
            break;
        }
    }
    if(!found)
        *fontStruct=NULL;

    return(found);
}


/*
    PsychGetFontRecordFromFontNameAndFontStyle()
    
    Accept a font family name and a font style.  Lookup the specified font in the list of font records.  If the font is found in the 
    list then set the fontStruct pointer to point to the font record.  Otherwise set fontStruct to NULL and return FALSE.    
*/
Boolean	PsychGetFontRecordFromFontFamilyNameAndFontStyle(char *fontFamilyName, FMFontStyle fontStyle, PsychFontStructType **fontStruct)
{
    PsychFontStructPtrType	current;
    Boolean			nameMatch, styleMatch, found;

    found=0;
    for(current=PsychGetFontListHead();  current; current=current->next){
        nameMatch= !(strcmp(current->fontFMFamilyName, fontFamilyName));
        styleMatch= current->fontFMStyle==fontStyle;
        found= nameMatch && styleMatch;
        if(found){
            *fontStruct=current;
            break;
        }
    }
    if(!found)
        *fontStruct=NULL;
    return(found);
}


/*
    PsychGetNumFontRecordsFromFontFamilyName()
    
    Accept a font family name string and a pointer to strorage.  Set the contents of the storage to hold 
    an array of pointers to font records for the fonts which are members of the  specified font family.
    
    Return the number of elements.
    
    Passing NULL in for pFontStructArray will cause PsychGetFontRecordsFromFontFamilyName() to return the tally of 
    the family's member fonts without storing the pointers. To determine the size of the pFontStructArray array 
    storage, first call PsychGetFontRecordsFromFontFamilyName, passing NULL in for pFontStructArray.
    
*/
int	PsychMemberFontsFromFontFamilyName(char *fontFamilyName, PsychFontStructPtrType *pFontStructArray)
{
    int				i;
    Boolean			nameMatch;
    PsychFontStructPtrType	current;
    
    i=0;
    for(current=PsychGetFontListHead();  current; current=current->next){
        nameMatch= !(strcmp(current->fontFMFamilyName, fontFamilyName));
        if(nameMatch){
            if(pFontStructArray)
                pFontStructArray[i]=current;
            i++;
        }
    }
    return(i);
}


/*
   PsychCopyFontRecordToNativeStruct()
   
   Accept an array of pointers to psych font structures and the lenght of the array.  
   Allocate and fill a native font struct array for the outside scripting environment   
*/    
void PsychCopyFontRecordsToNativeStructArray(int numFonts, PsychFontStructType **fontStructs, PsychGenericScriptType **fontStructArray)  
{
    int				i, arrayIndex;
    PsychFontStructPtrType	fontElement;
    PsychGenericScriptType	*styleNameList;
    Str255			styleName;
    //for the outer font struct
    int				numFontFieldNames=10;
    const char 			*fontFieldNames[]={"number", "name", "QuickDrawName", "PostScriptName", "familyName", "styleCode", 
                                                            "styleNames", "file",  "metrics", "locale" };
    //for the metrics meta struct
    int				numMetaMetricsFieldNames=2;
    const char 			*metaMetricsFieldNames[]={"verticalMetrics", "horizontalMetrics"};
    PsychGenericScriptType	*metaMetrics;
    //for the metrics structs
    int				numMetricsFieldNames=13;
    const char 			*metricsFieldNames[]={"ascent", "descent", "leading", "avgAdvanceWidth", "minLeftSideBearing", "minRightSideBearing",
                                                    "stemWidth","stemHeight","capHeight","xHeight","italicAngle","underlinePosition","underlineThickness" };
    PsychGenericScriptType	*horizontalMetrics, *verticalMetrics;
    //for locale
    int				numLocaleFieldNames=5;
    const char 			*localeFieldNames[]={"language", "languageVariant", "region", "regionVariant", "fullName"};
    PsychGenericScriptType	*locale;

    
    //convert the linked list of font structures to a struct array for the scripting environment.
    PsychAllocOutStructArray(kPsychNoArgReturn, FALSE, numFonts, numFontFieldNames, fontFieldNames, fontStructArray);
    for(arrayIndex=0; arrayIndex<numFonts; ++arrayIndex){
        fontElement=fontStructs[arrayIndex];
        PsychSetStructArrayDoubleElement("number", arrayIndex, fontElement->fontNumber, *fontStructArray);
        PsychSetStructArrayStringElement("name", arrayIndex, fontElement->fontFMName, *fontStructArray);
        PsychSetStructArrayStringElement("QuickDrawName", arrayIndex, fontElement->fontFamilyQuickDrawName, *fontStructArray);
        PsychSetStructArrayStringElement("PostScriptName", arrayIndex, fontElement->fontPostScriptName, *fontStructArray);
        PsychSetStructArrayStringElement("familyName", arrayIndex, fontElement->fontFMFamilyName, *fontStructArray);
        //style
        PsychSetStructArrayDoubleElement("styleCode", arrayIndex, (double)fontElement->fontFMStyle, *fontStructArray);
        PsychAllocOutCellVector(kPsychNoArgReturn, FALSE, fontElement->fontFMNumStyles,  &styleNameList);
        for(i=0;i<fontElement->fontFMNumStyles;i++){
            PsychGetFMFontStyleNameFromIndex(i, fontElement->fontFMStyle, styleName, 255); 
            PsychSetCellVectorStringElement(i, styleName, styleNameList);
        }
        PsychSetStructArrayNativeElement("styleNames", arrayIndex, styleNameList, *fontStructArray);
        PsychSetStructArrayStringElement("file", arrayIndex, fontElement->fontFile, *fontStructArray);
        //metrics
        PsychAllocOutStructArray(kPsychNoArgReturn, FALSE, 1, numMetaMetricsFieldNames, metaMetricsFieldNames,  &metaMetrics);
        PsychAllocOutStructArray(kPsychNoArgReturn, FALSE, 1, numMetricsFieldNames, metricsFieldNames, &horizontalMetrics);
        PsychAllocOutStructArray(kPsychNoArgReturn, FALSE, 1, numMetricsFieldNames, metricsFieldNames,  &verticalMetrics);
            //horizontal
            PsychSetStructArrayDoubleElement("ascent", 0, fontElement->horizontalMetrics.ascent, horizontalMetrics);
            PsychSetStructArrayDoubleElement("descent", 0, fontElement->horizontalMetrics.descent, horizontalMetrics);
            PsychSetStructArrayDoubleElement("leading", 0, fontElement->horizontalMetrics.leading, horizontalMetrics);
            PsychSetStructArrayDoubleElement("avgAdvanceWidth", 0, fontElement->horizontalMetrics.avgAdvanceWidth, horizontalMetrics);
            PsychSetStructArrayDoubleElement("minLeftSideBearing", 0, fontElement->horizontalMetrics.minLeftSideBearing, horizontalMetrics);
            PsychSetStructArrayDoubleElement("minRightSideBearing", 0, fontElement->horizontalMetrics.minRightSideBearing, horizontalMetrics);
            PsychSetStructArrayDoubleElement("stemWidth", 0, fontElement->horizontalMetrics.stemWidth, horizontalMetrics);
            PsychSetStructArrayDoubleElement("stemHeight", 0, fontElement->horizontalMetrics.stemHeight, horizontalMetrics);
            PsychSetStructArrayDoubleElement("capHeight", 0, fontElement->horizontalMetrics.capHeight, horizontalMetrics);
            PsychSetStructArrayDoubleElement("xHeight", 0, fontElement->horizontalMetrics.xHeight, horizontalMetrics);
            PsychSetStructArrayDoubleElement("italicAngle", 0, fontElement->horizontalMetrics.italicAngle, horizontalMetrics);
            PsychSetStructArrayDoubleElement("underlinePosition", 0, fontElement->horizontalMetrics.underlinePosition, horizontalMetrics);
            PsychSetStructArrayDoubleElement("underlineThickness", 0, fontElement->horizontalMetrics.underlineThickness, horizontalMetrics);
            //vertical
            PsychSetStructArrayDoubleElement("ascent", 0, fontElement->verticalMetrics.ascent, verticalMetrics);
            PsychSetStructArrayDoubleElement("descent", 0, fontElement->verticalMetrics.descent, verticalMetrics);
            PsychSetStructArrayDoubleElement("leading", 0, fontElement->verticalMetrics.leading, verticalMetrics);
            PsychSetStructArrayDoubleElement("avgAdvanceWidth", 0, fontElement->verticalMetrics.avgAdvanceWidth, verticalMetrics);
            PsychSetStructArrayDoubleElement("minLeftSideBearing", 0, fontElement->verticalMetrics.minLeftSideBearing, verticalMetrics);
            PsychSetStructArrayDoubleElement("minRightSideBearing", 0, fontElement->verticalMetrics.minRightSideBearing, verticalMetrics);
            PsychSetStructArrayDoubleElement("stemWidth", 0, fontElement->verticalMetrics.stemWidth, verticalMetrics);
            PsychSetStructArrayDoubleElement("stemHeight", 0, fontElement->verticalMetrics.stemHeight, verticalMetrics);
            PsychSetStructArrayDoubleElement("capHeight", 0, fontElement->verticalMetrics.capHeight, verticalMetrics);
            PsychSetStructArrayDoubleElement("xHeight", 0, fontElement->verticalMetrics.xHeight, verticalMetrics);
            PsychSetStructArrayDoubleElement("italicAngle", 0, fontElement->verticalMetrics.italicAngle, verticalMetrics);
            PsychSetStructArrayDoubleElement("underlinePosition", 0, fontElement->verticalMetrics.underlinePosition, verticalMetrics);
            PsychSetStructArrayDoubleElement("underlineThickness", 0, fontElement->verticalMetrics.underlineThickness, verticalMetrics);
        PsychSetStructArrayNativeElement("horizontalMetrics", 0, horizontalMetrics, metaMetrics);
        PsychSetStructArrayNativeElement("verticalMetrics", 0, verticalMetrics, metaMetrics);
        PsychSetStructArrayNativeElement("metrics", arrayIndex, metaMetrics, *fontStructArray);
        //locale
        PsychAllocOutStructArray(kPsychNoArgReturn, FALSE, 1, numLocaleFieldNames, localeFieldNames,  &locale);
            PsychSetStructArrayStringElement("language", 0, fontElement->locale.language, locale);
            PsychSetStructArrayStringElement("languageVariant", 0, fontElement->locale.languageVariant, locale);
            PsychSetStructArrayStringElement("region", 0, fontElement->locale.region, locale);
            PsychSetStructArrayStringElement("regionVariant", 0, fontElement->locale.regionVariant, locale);
            PsychSetStructArrayStringElement("fullName", 0, fontElement->locale.fullName, locale);
        PsychSetStructArrayNativeElement("locale", arrayIndex, locale, *fontStructArray);
    }
    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//For dealing with Font Manager font styles.

#define		kPsychFMMaxStyles	8		//the max number of non-normal styles, the maximum index of FontStyleNameTable[].  
static char *FontStyleNameTable[] = {"normal",	"bold",	"italic", "underline", "outline", "condense", "extend",	"unknownStyle1", "unknownStyle2"};

/*  
    PsychFindNumFMFontStylesFromStyle()

    The minimum is 0 which means "normal" style, for which there is no flag.  (It is implied by the absense of other flags.)
    
*/
int	PsychFindNumFMFontStylesFromStyle(FMFontStyle fmStyleFlag)
{
    int			i, numStyles;
    FMFontStyle		testBit;
    
    numStyles=0;
    testBit=1;
    for(i=0;i<8;i++){
        testBit=1<<i;
        if(fmStyleFlag & testBit)
            ++numStyles;
    }
    return(numStyles);
}


 
                    
/*
    PsychGetFontStyleNameFromIndex()
    
    Get a string naming the nth font style which is set within an FMFontStyle value, where n is a number between 0  and the value
    returned by PsychFindNumFMFontStylesFromStyle() -1 when called on the FMFontStyle value.
    
    styleNameLength is the length of the string in characters, not the length in bytes.  Length in bytes = length in chars + 1.
    
*/
void PsychGetFMFontStyleNameFromIndex(int styleIndex, FMFontStyle fontStyle, char *styleName, int styleNameLength)
{
    int		i, totalStyles, styleCounter;
    
    
    //count up through the set bits until we reach the styleIndex then return the name of that bit position.
    //if we count to the end and find no bits then return "normal".  
    totalStyles=PsychFindNumFMFontStylesFromStyle(fontStyle);
    if(styleIndex > totalStyles)
        PsychErrorExitMsg(PsychError_internal, "Index value exceeds maximum");
    styleCounter=-1;
    for(i=0;i<=kPsychFMMaxStyles;i++){
        if( 1<<i & fontStyle)
            ++styleCounter;
        if(styleCounter == styleIndex){
            strncpy(styleName, FontStyleNameTable[i+1], styleNameLength);
            styleName[styleNameLength]='\0';
            return;
        }
    }
    if(styleIndex != 0)	
        //if the style is normal then there can be only one style and its index must be 0.    
        PsychErrorExitMsg(PsychError_internal, "indexed style not present in style value");
    strncpy(styleName, FontStyleNameTable[0], styleNameLength);
    styleName[styleNameLength]='\0';
    return;
} 




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//For ATSU

/*
	PsychSetATSUTStyleAttributesFromPsychFontRecord()
	
	Use the Font Manager attributes for the given window, which are stored in the PsychFontAtributeStruct within the window's record, to set the 
	attributes of the ATSU style.
	
	For now we only set the font ID, the color, and the size. Boldness, underline, condensed and extended we assume are brought along
	with the font ID , though perhaps not and we will have to set those also that.
	
	PsychSetATSUTStyleAttributes() requires that the font number within the specified window record indicate a
	valid font.  
*/
/*
void SuperFooBar(void *atsuStyleFoo,  PsychWindowRecordType *winRec)
{
	PsychFontStructType		*psychFontRecord;
	int						psychColorSize;
	//for ATSU attributes
	ATSUFontID				atsuFontID;
	Fixed					atsuFontSize;
	ATSURGBAlphaColor		atsuFontColor;
	GLdouble				colorVector[4];
	OSStatus				callError;
	ATSUAttributeTag		aaTags[] =  {kATSUFontTag, kATSUSizeTag, kATSURGBAlphaColorTag };
	ByteCount				aaSizes[] = {sizeof(ATSUFontID), sizeof(Fixed), sizeof(ATSURGBAlphaColor) };
	ATSUAttributeValuePtr   aaValue[] = {&atsuFontID, &atsuFontSize, &atsuFontColor};
	
	//set the font index
	PsychGetFontRecordFromFontNumber(winRec->textAttributes.textFontNumber, &psychFontRecord);
	atsuFontID=psychFontRecord->fontFMRef;
	//set the font size
	atsuFontSize=Long2Fix((long)(winRec->textAttributes.textSize));
	//set the color
	psychColorSize=PsychGetColorSizeFromWindowRecord(winRec);
	PsychCoerceColorModeWithDepthValue(kPsychRGBAColor, psychColorSize, &(winRec->textAttributes.textColor));
	PsychConvertColorAndColorSizeToDoubleVector(&(winRec->textAttributes.textColor), psychColorSize, colorVector);
	atsuFontColor.red=(float)colorVector[0];
	atsuFontColor.green=(float)colorVector[1];
	atsuFontColor.blue=(float)colorVector[2];
	atsuFontColor.alpha=(float)colorVector[3];
	//assign attributes to the style object
	callError=ATSUSetAttributes(atsuStyle, 3, aaTags, aaSizes, aaValue);
	
}
    
*/



