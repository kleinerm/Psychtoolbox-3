/*
  PsychToolbox2/Source/OSX/Fonts/FONTSFonts.c		
  
  PROJECTS: Fonts only.  
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	Mac OS X Only
    

  HISTORY:
  1/07/04	awi		Created. Derived from the earlier ATS incarnation.
 
  
  DESCRIPTION: 
  
  NOTES:
  
  It's not clear with what to fill the "Name" field of the returned struct because a font has so many names.  Ultimately we want the names
  to match on all platforms, so perhaps we should wait to see which names are available in Windows and then choose one wich is available on both
  platforms.  For now we use the Font Manager name.
  

*/ 

#include "FontInfo.h"


static char useString[] = "fontList=Fonts('Fonts')";
static char synopsisString[] = 
	"Returns a list struct holding ";
static char seeAlsoString[] = "NumFonts";


/*

typedef struct _PsychFontLocaleStructType_{
        Str255			language;
        Str255			languageVariant;
        Str255			region;
        Str255			regionVariant;
        Str255			fullName;
} PsychFontLocaleStructType;


        
        
        
typedef struct _PsychFontMetricsStructType_{
        double			ascent;
        double			descent;
        double		        leading;
        double			avgAdvanceWidth;
        double			minLeftSideBearing;
        double			minRightSideBearing;
        double      		stemWidth;
        double     		stemHeight;
        double      		capHeight;
        double        		xHeight;
        double    		italicAngle;
        double			underlinePosition;
        double			underlineThickness;
} PsychFontMetricsStructType;    


typedef struct _PsychFontStructType_{

        //list management
        int					fontNumber;
        PsychFontStructPtrType			next;
 
        //Names associated with the font.  We could also add the ATS name and the 
        Str255					fontFMName;
        Str255					fontFMFamilyName;
        Str255					fontFamilyQuickDrawName;
        Str255					fontPostScriptName;
        char					fontFile[kPsychMaxFontFileNameChars];  	
			
        //depricated font style stuff for compatability with OS 9 scripts
        FMFontStyle				fontFMStyle;
        int					fontFMNumStyles;
        
        //Retain the the Font Manager (FM) and Apple Type Services (ATS) references to the font.
        //We could use these to tie font families to font names, for example 
        // Fonts('GetFontsFromFontFamilyName') or Fonts('GetFontFamilyFromFont');
        ATSFontRef				fontATSRef;
        FMFont                                 	fontFMRef;
        ATSFontFamilyRef			fontFamilyATSRef;
        FMFontFamily				fontFamilyFMRef;
        
        //font's language and country
        PsychFontLocaleStructType		locale;			
        
        //font metrics
        PsychFontMetricsStructType		horizontalMetrics;
        PsychFontMetricsStructType		verticalMetrics;
} PsychFontStructType;

*/
	 

PsychError FONTSFonts(void) 
{
    PsychGenericScriptType 	*nativeStructArray;
    int				arrayIndex, numFonts;
    PsychFontStructType		**fontPointerList, *fontElement;
    
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    //check for  required and superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));
        
    //build a list of pointers to all the the font records in the font list, then hand the list of pointers to PsychCopyFontRecordsToNativeStructArray()
    //to get a native struct array of font records and return it to the scripting environment.
    arrayIndex=0;
    numFonts=PsychGetFontListLength();
    fontPointerList=(PsychFontStructType**)malloc(numFonts * sizeof(PsychFontStructType*));
    for(fontElement=PsychGetFontListHead();fontElement;fontElement=fontElement->next)
        fontPointerList[arrayIndex++]=fontElement;
    PsychCopyFontRecordsToNativeStructArray(numFonts, fontPointerList, &nativeStructArray);  
    free((void*)fontPointerList);
    PsychAssignOutStructArray(1, FALSE, nativeStructArray);
    
    return(PsychError_none);	
}


/*
PsychError FONTSFonts(void) 
{
    int				i, numFonts, arrayIndex;
    PsychFontStructPtrType	fontElement;
    PsychGenericScriptType	*styleNameList;
    Str255			styleName;
    //for the outer font struct
    int				numFontFieldNames=10;
    const char 			*fontFieldNames[]={"number", "name", "QuickDrawName", "PostScriptName", "familyName", "styleCode", 
                                                            "styleNames", "file",  "metrics", "locale" };
    PsychGenericScriptType	*fontStructArray;
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

    
	
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    //check for  required and superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));
        
    //convert the linked list of font structures to a struct array for the scripting environment.
    arrayIndex=0;
    numFonts=PsychGetFontListLength();
    PsychAllocOutStructArray(1, FALSE, numFonts, numFontFieldNames, fontFieldNames, &fontStructArray);
    for(fontElement=PsychGetFontListHead();fontElement;fontElement=fontElement->next){
        PsychSetStructArrayDoubleElement("number", arrayIndex, fontElement->fontNumber, fontStructArray);
        PsychSetStructArrayStringElement("name", arrayIndex, fontElement->fontFMName, fontStructArray);
        PsychSetStructArrayStringElement("QuickDrawName", arrayIndex, fontElement->fontFamilyQuickDrawName, fontStructArray);
        PsychSetStructArrayStringElement("PostScriptName", arrayIndex, fontElement->fontPostScriptName, fontStructArray);
        PsychSetStructArrayStringElement("familyName", arrayIndex, fontElement->fontFMFamilyName, fontStructArray);
        //style
        PsychSetStructArrayDoubleElement("styleCode", arrayIndex, (double)fontElement->fontFMStyle, fontStructArray);
        PsychAllocOutCellVector(kPsychNoArgReturn, FALSE, fontElement->fontFMNumStyles,  &styleNameList);
        for(i=0;i<fontElement->fontFMNumStyles;i++){
            PsychGetFMFontStyleNameFromIndex(i, fontElement->fontFMStyle, styleName, 255); 
            PsychSetCellVectorStringElement(i, styleName, styleNameList);
        }
        PsychSetStructArrayNativeElement("styleNames", arrayIndex, styleNameList, fontStructArray);
        PsychSetStructArrayStringElement("file", arrayIndex, fontElement->fontFile, fontStructArray);
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
        PsychSetStructArrayNativeElement("metrics", arrayIndex, metaMetrics, fontStructArray);
        //locale
        PsychAllocOutStructArray(kPsychNoArgReturn, FALSE, 1, numLocaleFieldNames, localeFieldNames,  &locale);
            PsychSetStructArrayStringElement("language", 0, fontElement->locale.language, locale);
            PsychSetStructArrayStringElement("languageVariant", 0, fontElement->locale.languageVariant, locale);
            PsychSetStructArrayStringElement("region", 0, fontElement->locale.region, locale);
            PsychSetStructArrayStringElement("regionVariant", 0, fontElement->locale.regionVariant, locale);
            PsychSetStructArrayStringElement("fullName", 0, fontElement->locale.fullName, locale);
        PsychSetStructArrayNativeElement("locale", arrayIndex, locale, fontStructArray);
        ++arrayIndex;
    }
    return(PsychError_none);	
}

*/












