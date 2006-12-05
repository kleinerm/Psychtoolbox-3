/*
	PsychToolbox2/Source/OSX/Fonts/ScreenFontGlue.c
	
	PLATFORMS:	This is the OS X  version.  
				
	AUTHORS:
	
		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	
		7/8/04		awi		Split it of from PsychFontGlue because PsychWindowRecordType in PsychSetATSUStyleAttributes
							was causing trouble for the FontInfo project
							
	DESCRIPTION:
        
*/

#include "ScreenFontGlue.h"

/*
	PsychSetATSUStyleAttributesFromPsychWindowRecord()
	
	Use the Font Manager attributes for the given window, which are stored in the PsychFontAtributeStruct within the window's record, to set the 
	attributes of the ATSU style.
	
	For now we only set the font ID, the color, and the size. Boldness, underline, condensed and extended we assume are brought along
	with the font ID , though perhaps not and we will have to set those also that.
	
	PsychSetATSUStyleAttributes() requires that the font number within the specified window record indicate a
	valid font.  
*/

void PsychSetATSUStyleAttributesFromPsychWindowRecord(ATSUStyle atsuStyle,  PsychWindowRecordType *winRec)
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
	if(psychFontRecord==NULL)
		PsychErrorExitMsg(PsychError_internal, "Failed to lookup the font from the font number");
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
    




