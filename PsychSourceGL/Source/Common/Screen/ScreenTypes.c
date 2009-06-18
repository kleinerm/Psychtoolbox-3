/*
	PsychToolbox3/Source/Common/Screen/ScreenTypes.cpp
	
	PLATFORMS:	Windows
				MacOS9
			
	
	AUTHORS:
	Allen Ingling	awi		Allen.Ingling@nyu.edu
	Mario Kleiner	mk		mario.kleiner at tuebingen.mpg.de

	HISTORY:
	09/09/02		awi		wrote it.  
	11/14/06        mk      Rewritten to support 10bpc, float framebuffers.
	01/06/07		mk		Purged much of the bullshit it was.

	DESCRIPTION:
	
	functions which operate on types defined in ScreenTypes.h	
	
	TODO:
	
	Most of this stuff should just die. It doesn't match how OpenGL works.
	--> Done. Killed as much as possible.
*/

#include "Screen.h"

//accessors for PsychDepthType
 
void PsychInitDepthStruct(PsychDepthType *depth)
{
    depth->numDepths = 0;
}

int PsychGetNumDepthsFromStruct(PsychDepthType *depth)
{
    return(depth->numDepths);
}


int PsychGetValueFromDepthStruct(int index, PsychDepthType *depth)
{
    
    if(depth->numDepths<=0 || index >= depth->numDepths){
        PsychErrorExitMsg(PsychError_internal,"Invalid depth index passed to PsychGetDepthFromStruct()");
        return(-1); //makes the compiler happy.  
    }
    else 
        return(depth->depths[index]); 
       
}


void PsychAddValueToDepthStruct(int value, PsychDepthType *depth)
{
    int i;
    
    //check to see if the value is already in the list
    for(i=0;i<depth->numDepths;i++){
        if(depth->depths[i] == value)
            return;
    }
    //check to see if we have space left
    if(depth->numDepths==kPsychMaxPossiblePixelDepths)
        PsychErrorExitMsg(PsychError_internal,"Capacity of PsychDepthType struture exceeded");
    //increment the count and store the new value
    depth->depths[depth->numDepths++] = value;
}


/*
    PsychDepthIsMember()
    
    Accepts two depth structs and returns true if the one depth held by the first struct is among the one or more 
    depths within the second struct. 
*/
psych_bool PsychIsMemberDepthStruct(PsychDepthType *depth, PsychDepthType *depthSet)
{
    int numDepths, i;
    
    numDepths=PsychGetNumDepthsFromStruct(depth);
    if(numDepths>1)
        PsychErrorExitMsg(PsychError_internal, "depth structure contains multiple depths");
    else if(numDepths==0)
        return(FALSE);
    for(i=0;i<PsychGetNumDepthsFromStruct(depthSet);i++){
        if(PsychGetValueFromDepthStruct(i,depthSet) == PsychGetValueFromDepthStruct(0,depth))
            return(TRUE);
    }
    return(FALSE);
}

/*
    PsychDepthCopy()
    
    Accepts two depth structs which must both have been initialized and copies the contents of the second
    depth to the first. Copy is cumulative. 
*/
void PsychCopyDepthStruct(PsychDepthType *toDepth, PsychDepthType *fromDepth)
{
    int i;
    
    for(i=0;i<PsychGetNumDepthsFromStruct(fromDepth); i++)
        PsychAddValueToDepthStruct(PsychGetValueFromDepthStruct(i,fromDepth), toDepth);
}


/*
    PsychGetColorModeFromDepthStruct()
	MK: This is a useless relict from old times. It always returns RGBA color mode,
	the only mode we will ever support...
*/ 
PsychColorModeType PsychGetColorModeFromDepthStruct(PsychDepthType *depth)
{
	return(kPsychRGBAColor);
}

// Return the white value that really corresponds to white for a specific
// window, regardless of color depths and whatever...
double PsychGetWhiteValueFromWindow(PsychWindowRecordType* windowRecord)
{
	return(fabs(windowRecord->colorRange));
}

/*
    PsychLoadColorStruct(): Internal method to set colors, usually used to
	setup default colors.
	
	CAUTION: Only accepts double values, not int's, does not automatically
	cast, but screws up insted due to the va_arg macro magic below...
    
    PsychLoadColorStruct is polymorphic:
    PsychLoadColorStruct(*color, kPsychIndexColor,  int index)
    PsychLoadColorStruct(*color, kPsychRGBColor, int r, int g, int b)
    PsychLoadColorStruct(*color, kPsychRGBAColor,  int r, int g, int b, int a)
*/
void PsychLoadColorStruct(PsychColorType *color, PsychColorModeType mode,  ...)
{
    va_list ap;
    
    color->mode=mode;
    va_start(ap, mode);
    switch(mode){
        case kPsychIndexColor:
            color->value.index.i=va_arg(ap,double);
            break;
        case kPsychRGBColor:
            color->value.rgb.r=va_arg(ap,double);
            color->value.rgb.g=va_arg(ap,double);
            color->value.rgb.b=va_arg(ap,double);
            break;
        case kPsychRGBAColor:
            color->value.rgba.r=va_arg(ap,double);
            color->value.rgba.g=va_arg(ap,double);
            color->value.rgba.b=va_arg(ap,double);
            color->value.rgba.a=va_arg(ap,double);
            break;
        case kPsychUnknownColor:
            PsychErrorExitMsg(PsychError_internal,"Unspecified display mode");
    }
            
    va_end(ap);        
        
}

/*
    PsychCoerceColorMode()
    
	Converts any color mode into RGBA mode, the only mode we need to handle with OpenGL,
	regardless of display depth, number of color planes or whatever. The GL will always
	accept the full thing and discard unwanted information internally in a much more fool
	proof way than we could do it.
	
	MK: Important change! We now assign DBL_MAX for unknown alphas and our color setup routine
	(see SetGLColor()) maps DBL_MAX to the maximum supported value, which is simple, because
	it always passes colors as doubles via glColor4d, so the maximum is always 1.0, irrespective
	of display depth.
	
*/
void PsychCoerceColorMode(PsychColorType *color)
{
    double index;

	// Conversion from Index color?
    if(color->mode==kPsychIndexColor) {
		// Replicate index color value into RGB channels:
        index=color->value.index.i;
        color->value.rgba.r=index;
        color->value.rgba.g=index;
        color->value.rgba.b=index;
		// Set alpha to our "maximum alpha" value:
        color->value.rgba.a=DBL_MAX;
	}
	else {
		// No, conversion from RGB?
		if(color->mode==kPsychRGBColor) {
			// Yes. Copy RGB channels over:
			color->value.rgba.r=color->value.rgb.r;
			color->value.rgba.g=color->value.rgb.g;
			color->value.rgba.b=color->value.rgb.b;
			// Set alpha to our "maximum alpha" value:
			color->value.rgba.a=DBL_MAX;
		}
		// Third case would be from RGBA to RGBA, but that's
		// a no-op.
	}
	
	// Always convert to RGBA color mode:
	color->mode=kPsychRGBAColor;
}
