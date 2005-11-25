/*
	PsychToolbox2/Source/Common/Screen/ScreenTypes.h
	
	PLATFORMS:	Windows
				MacOS9
			
	
	AUTHORS:
	Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	09/06/02		awi		wrote it.  
        11/26/02		awi			turned PsychDepthType into an int from enum.  
        
	
	DESCRIPTION:
	
	Platform-neutral Typedefs for the Screen Psychtoolbox project.  This file should appear ahead of all 
        other files on the Screen.h include list.  
        
        This file should contain only platform-neutral typedefs.  Platform specfic stuff all goes into
        files within PsychToolbox3/Source/PLATFORMNAME/Screen/.  Currently those files are PsychSurfaceGlue.c
        and PsychSurfaceGlue.h
	
	TO DO: 
	
        -The depth stuff is stupid.  Fix it: break out the number of values and an array from that dumb structure and abolish the structure.
        
        -PsychColorModeType seems mostly useless, it contains no more information than the depth alone.  If it is always the case that:
        8-bits = indexed mode
        16-bits = 5 bits of red , 5 bits of green 5 bits of blue.
        24-bits = 8 bits of red, 8 bits of green, 8 bits of blue.
        32-bits = 8 bits of red, 8 bits of green, 8 bits of blue.  
        Then we don't need the mode as a seperate and purely redundant representation.  However, I think somewhere a DirectX structure defined an 8-bit direct mode,
        so we might want to consider verifying that before abolishing PsychColorModeType.  Still, it might be better to have the depth and like PsychIsColorModeIndexed
        instead of PsychColorModeType and depth, because the former is more orthagonal.    
	

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_ScreenTypes
#define PSYCH_IS_INCLUDED_ScreenTypes

//constants
#define 	kPsychMaxPossiblePixelDepths		10

//includes 
#include "Screen.h"


//project typedefs
typedef int PsychWindowIndexType;
typedef int PsychNumdexType; 		  //"numdex" is screen NUMber or window inDEX.
typedef int PsychDepthArrayType[kPsychMaxPossiblePixelDepths];


typedef struct {
        int						numDepths;
        PsychDepthArrayType		depths;
        } PsychDepthType;
        
typedef enum {
        kPsychFrontBuffer,
        kPsychBackBuffer,
} PsychWindowBufferType;
        		

typedef enum {
	kPsychNoWindow =				0,
	kPsychAnyWindow =				1,			//  specify if we want to match to any window type.
	kPsychSingleBufferOnscreen = 	2,	
	kPsychDoubleBufferOnscreen = 	3,	  
	kPsychVideoMemoryOffscreen = 	4,
	kPsychSystemMemoryOffscreen = 	5,
	kPsychTexture =					6
	} PsychWindowType; //naming conventions dictate "PsychWindowTypeType" but that's silly.


typedef enum {
	kPsychUnknownColor = 0,
	kPsychRGBColor,			//means PsychRGBColorType
	kPsychRGBAColor,		//means RGBColorType with alpha channel
	kPsychIndexColor		//means PsychIndexColorType
}	PsychColorModeType;

typedef struct {
	int r;
	int g;
	int b;
	int a;		//alpha
} PsychRGBAColorType;

typedef struct {
	int r;
	int g;
	int b;
} PsychRGBColorType;


typedef struct {
	int i;	//color index   	
} PsychIndexColorType;


typedef struct {
	PsychColorModeType mode;
	union {
		PsychRGBAColorType		rgba;
		PsychRGBColorType		rgb;
		PsychIndexColorType 	index;
	} value;
} PsychColorType;
 
typedef struct {
        int					screenNumber; 
        PsychRectType 		rect;
        PsychDepthType 		depth;
        PsychColorModeType	mode;
        int					nominalFrameRate;
		//int				dacbits;
} PsychScreenSettingsType;

typedef struct {
		int						foo;
		//put more screen info here. 
		//this is 
} PsychScreenRecordType;     


//indices into 3D or 4D matrix array returned by GetImage
#define kPsychRedPlane		0
#define kPsychGreenPlane	1
#define kPsychBluePlane		2
#define kPsychAlphaPlane	3
#define kPsychIndexPlane	0

// Constants that can be or'ed together for the "ConserveVRAM"
// Screen Preference setting:
// Don't use AUX buffers, disable associated features.
#define kPsychDisableAUXBuffers 1
// Use Client storage for RAM->DMA texturing instead of VRAM texturing.
#define kPsychDontCacheTextures 2


//function protoptypes

//Accessors for PsychDepthType 
void				PsychInitDepthStruct(PsychDepthType *depth);
int					PsychGetNumDepthsFromStruct(PsychDepthType *depth);
int					PsychGetValueFromDepthStruct(int index, PsychDepthType *depth);
void				PsychAddValueToDepthStruct(int value, PsychDepthType *depth);
boolean				PsychIsMemberDepthStruct(PsychDepthType *depth, PsychDepthType *depthSet);
void				PsychCopyDepthStruct(PsychDepthType *toDepth, PsychDepthType *fromDepth);
PsychColorModeType 	PsychGetColorModeFromDepthStruct(PsychDepthType *depth);
PsychColorModeType 	PsychGetColorModeFromDepthValue(int depthValue);
int					PsychGetWhiteValueFromDepthStruct(PsychDepthType *depth);
int					PsychGetWhiteValueFromDepthValue(int depthValue);
int					PsychGetColorSizeFromDepthValue(int depthValue);
int					PsychGetNumPlanesFromDepthValue(int depthValue);

//Accessors for PsychColorType
void				PsychLoadColorStruct(PsychColorType *color, PsychColorModeType mode,  ...);
void				PsychCoerceColorModeFromSizes(int numColorPlanes, int colorPlaneSize, PsychColorType *color);
void				PsychCoerceColorMode(PsychColorModeType mode, PsychColorType *color);
void				PsychCoerceColorModeWithDepthValue(PsychColorModeType mode, int depthValue, PsychColorType *color);


//end include once
#endif
