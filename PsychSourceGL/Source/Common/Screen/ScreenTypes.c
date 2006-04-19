/*
	PsychToolbox2/Source/Common/Screen/ScreenTypes.cpp
	
	PLATFORMS:	Windows
				MacOS9
			
	
	AUTHORS:
	Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	09/09/02			awi		wrote it.  
	
	DESCRIPTION:
	
	functions which operate on types defined in ScreenTypes.h	
	

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
boolean PsychIsMemberDepthStruct(PsychDepthType *depth, PsychDepthType *depthSet)
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
    
    Get ride of this color mode stuff and replace it with functions wich access the number of depth planes and the size of every plane.
*/ 
PsychColorModeType PsychGetColorModeFromDepthStruct(PsychDepthType *depth)
{
switch(PsychGetValueFromDepthStruct(0, depth)){
        case 8:	
            return(kPsychIndexColor);
        case 16:
	    return(kPsychRGBAColor);    // MK: Changed. We always want RGBA mode. OpenGL can handle this even for 16-bit and 24-bit case.
        case 24:
            return(kPsychRGBAColor);    // MK: Changed. We always want RGBA mode. OpenGL can handle this even for 16-bit and 24-bit case.
        case 32:
            return(kPsychRGBAColor);
        default:
            return(kPsychUnknownColor);
    }
}

PsychColorModeType PsychGetColorModeFromDepthValue(int depthValue)
{
    PsychDepthType depth;
    
    PsychInitDepthStruct(&depth);
    PsychAddValueToDepthStruct(depthValue, &depth);
    return(PsychGetColorModeFromDepthStruct(&depth));
}


int PsychGetWhiteValueFromDepthStruct(PsychDepthType *depth)
{
    switch(PsychGetValueFromDepthStruct(0, depth)){
        case 8:	
            return(255);
        case 16:
            return(31);
        case 24:
            return(255);
        case 32:
            return(255);
        default:
            PsychErrorExitMsg(PsychError_internal, "Unrecognized screen depth value");
            return(0);  //makes the compiler happy
    }
}


int PsychGetWhiteValueFromDepthValue(int depthValue)
{
    PsychDepthType depth;
    
    PsychInitDepthStruct(&depth);
    PsychAddValueToDepthStruct(depthValue, &depth);
    return(PsychGetWhiteValueFromDepthStruct(&depth));
}



int PsychGetColorSizeFromDepthValue(int depthValue)
{
    switch(depthValue){
        case 8:	
            return(8);
        case 16:
	    return(8);    // MK: Changed. We always assume RGBA8 pixel formats. OpenGL can handle this in 16-bit case.
        case 24:
            return(8);
        case 32:
            return(8);
        default:
            PsychErrorExitMsg(PsychError_internal, "Unrecognized screen depth value");
            return(0);  //makes the compiler happy
    }
}





int PsychGetNumPlanesFromDepthValue(int depthValue)
{
    switch(depthValue){
        case 8:	
            return(1);
        case 16:
	    return(4); // MK: Changed from 3 to 4. OpenGL can handle this by itself.
        case 24:
            return(4); // MK: Changed from 3 to 4. OpenGL can handle this by itself.
        case 32:
            return(4); 
        default:
            PsychErrorExitMsg(PsychError_internal, "Unrecognized screen depth value");
            return(0);  //makes the compiler happy
    }
}


/*
    PsychGetMaxValueFromColorSize()
    
*/
int PsychGetMaxValueFromColorSize(int colorPlaneSize)
{
    switch(colorPlaneSize){
        case 1:
            return(1);
        case 5:
            return(31);
        case 8:
            return(255);
        default:
            PsychErrorExitMsg(PsychError_internal, "Unrecognized screen depth value");
            return(0);  //makes the compiler happy 
    }
}





/*
    PsychLoadColorStruct()
    
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
            color->value.index.i=va_arg(ap,int);
            break;
        case kPsychRGBColor:
            color->value.rgb.r=va_arg(ap,int);
            color->value.rgb.g=va_arg(ap,int);
            color->value.rgb.b=va_arg(ap,int);
            break;
        case kPsychRGBAColor:
            color->value.rgba.r=va_arg(ap,int);
            color->value.rgba.g=va_arg(ap,int);
            color->value.rgba.b=va_arg(ap,int);
            color->value.rgba.a=va_arg(ap,int);
            break;
        case kPsychUnknownColor:
            PsychErrorExitMsg(PsychError_internal,"Unspecified display mode");
    }
            
    va_end(ap);        
        
}

    
 
 /*
    PsychCoerceColorModeFromSizes()
    
    Accepts a color specifier of arbitrary and convert it to a color specifier of the mode appropriate for the specified
	number of color planes and plane size.
    
    Not all conversions are necessary or supported.    
*/
void PsychCoerceColorModeFromSizes(int numColorPlanes, int colorPlaneSize, PsychColorType *color)
{

    //from index to RGB
    if(numColorPlanes==3 && color->mode==kPsychIndexColor){
        color->mode=kPsychRGBColor;
        color->value.rgb.r=color->value.index.i;
        color->value.rgb.g=color->value.index.i;
        color->value.rgb.b=color->value.index.i;
    //from index to RGBA
    }else if(numColorPlanes==4 && color->mode==kPsychIndexColor){
        color->mode=kPsychRGBAColor;
        color->value.rgba.r=color->value.index.i;
        color->value.rgba.g=color->value.index.i;
        color->value.rgba.b=color->value.index.i;
        color->value.rgba.a=PsychGetMaxValueFromColorSize(colorPlaneSize);
    //from index to RGB to RGBA
    }else if(numColorPlanes==4 && color->mode==kPsychRGBColor){
        color->mode=kPsychRGBAColor;
        color->value.rgba.a=PsychGetMaxValueFromColorSize(colorPlaneSize);
    }else if(numColorPlanes==3 && color->mode==kPsychRGBColor)
        ;
    else if(numColorPlanes==1 && color->mode==kPsychIndexColor)
        ;
    else if(numColorPlanes==4 && color->mode==kPsychRGBAColor)
        ;
    else if(numColorPlanes==3 && color->mode==kPsychRGBAColor)
      ; // MK: This is a no-op: We stay in RGBA mode if RGBA->RGB transition is requested.
    else
        PsychErrorExitMsg(PsychError_internal, "Error attempting to coerce color specifier");
}


/*
    PsychCoerceColor()
    
    Accepts a color specifier for one display mode and converts it into another.  This doesn't fill in  the right alpha value
	and we should use PsychCoerceColorModeWithDepth()
    
    Not all conversions are necessary or supported.    
*/
void PsychCoerceColorMode(PsychColorModeType mode, PsychColorType *color)
{
    int index;

    
    if(mode==kPsychRGBColor && color->mode==kPsychIndexColor){
        index=color->value.index.i;
        color->mode=kPsychRGBColor;
        color->value.rgb.r=index;
        color->value.rgb.g=index;
        color->value.rgb.b=index;
    }else if(mode==kPsychRGBAColor && color->mode==kPsychIndexColor){
        index=color->value.index.i;
        color->mode=kPsychRGBAColor;
        color->value.rgba.r=index;
        color->value.rgba.g=index;
        color->value.rgba.b=index;
        color->value.rgba.a=index;
    }else if(mode==kPsychRGBAColor && color->mode==kPsychRGBColor){
        color->mode=kPsychRGBAColor;
        color->value.rgba.r=color->value.rgb.r;
        color->value.rgba.g=color->value.rgb.g;
        color->value.rgba.b=color->value.rgb.b;
        color->value.rgba.a=255;
   }else if(mode==kPsychRGBColor && color->mode==kPsychRGBColor)
        ;
    else if(mode==kPsychIndexColor && color->mode==kPsychIndexColor)
        ;
    else if(mode==kPsychRGBAColor && color->mode==kPsychRGBAColor)
        ;
    else if(mode==kPsychRGBColor && color->mode==kPsychRGBAColor)
      ; // MK: This is a no-op: We stay in RGBA mode if RGBA->RGB transition is requested.
    else
        PsychErrorExitMsg(PsychError_internal, "Error attempting to coerce color specifier");
}


/*
    PsychCoerceColor()
    
    Accepts a color specifier for one display mode and converts it into another.  This doesn't fill in  the right alpha value
	and we should use PsychCoerceColorModeWithDepth()
    
    Not all conversions are necessary or supported.    
*/
void PsychCoerceColorModeWithDepthValue(PsychColorModeType mode, int depthValue, PsychColorType *color)
{
    int index, maxValue;
	
	
	maxValue=PsychGetWhiteValueFromDepthValue(depthValue);
    
    if(mode==kPsychRGBColor && color->mode==kPsychIndexColor){
        index=color->value.index.i;
        color->mode=kPsychRGBColor;
        color->value.rgb.r=index;
        color->value.rgb.g=index;
        color->value.rgb.b=index;
    }else if(mode==kPsychRGBAColor && color->mode==kPsychIndexColor){
        index=color->value.index.i;
        color->mode=kPsychRGBAColor;
        color->value.rgba.r=index;
        color->value.rgba.g=index;
        color->value.rgba.b=index;
        color->value.rgba.a=maxValue;
	}else if(mode==kPsychRGBAColor && color->mode==kPsychRGBColor){
        color->mode=kPsychRGBAColor;
        color->value.rgba.r=color->value.rgb.r;
        color->value.rgba.g=color->value.rgb.g;
        color->value.rgba.b=color->value.rgb.b;
        color->value.rgba.a=maxValue;
   }else if(mode==kPsychRGBColor && color->mode==kPsychRGBColor)
        ;
    else if(mode==kPsychIndexColor && color->mode==kPsychIndexColor)
        ;
    else if(mode==kPsychRGBAColor && color->mode==kPsychRGBAColor)
        ;
    else if(mode==kPsychRGBColor && color->mode==kPsychRGBAColor)
      ; // MK: This is a no-op: We stay in RGBA mode if RGBA->RGB transition is requested.
    else
        PsychErrorExitMsg(PsychError_internal, "Error attempting to coerce color specifier");
}
