/*
	Screen.h
	
	PLATFORMS:
			
		Only OS X for now.
			
	AUTHORS:
	
		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	
		12/18/01		awi		wrote it.  Derived from joystick.h
		11/16/04		awi		Added  SCREENGlobalRect 
		1/25/05			awi		Merged in mk version which provides ScreenCloseAllWindows() declaration. Then restored SCREENGetTimeList declaration.  
                4/22/05                 mk              Added new commands DrawLines, SelectStereoDrawBuffer and DrawingFinished
                5/09/05                 mk              Added new command GetFlipInterval
                10/11/05                mk              Support for special movie playback added (include of PsychMovieSupport.h)

 */

//begin include once 
#ifndef PSYCH_IS_INCLUDED_Screen
#define PSYCH_IS_INCLUDED_Screen

//project includes includes for screen foundation
#include "Psych.h"
#include "PsychRects.h"
#include "ScreenTypes.h"

#if PSYCH_SYSTEM == PSYCH_OSX
	#include <Quicktime/Movies.h>
#endif

#if PSYCH_SYSTEM == PSYCH_WINDOWS
	#include <Movies.h>
	#include <CoreVideo.h>
	#include <GXMath.h>
	#include <ImageCompression.h>
#endif

#include "PsychVideoGlue.h"
#include "PsychScreenGlue.h"
#include "PsychWindowTextGlue.h"
#include "WindowBank.h"
#include "PsychWindowGlue.h"
#include "PsychWindowSupport.h"
#include "PsychMovieSupport.h"
#include "PsychTextureSupport.h"
#include "PsychAlphaBlending.h"
#include "PsychVideoCaptureSupport.h"
#include "PsychImagingPipelineSupport.h"

#include "ScreenArguments.h"
#include "RegisterProject.h"
#include "WindowHelpers.h"

// The windows version is without any font or text drawing support :(
#if PSYCH_SYSTEM == PSYCH_OSX
#include "PsychFontGlue.h"
#include "ScreenFontGlue.h"
#include "FontInfo.h"
#endif


#include "ScreenPreferenceState.h"



//functions registered with the Psychtoolbox library 
PsychError ScreenExitFunction(void); 			//ScreenExit.c
PsychError PsychDisplayScreenSynopsis(void);	        //ScreenSynopsis.c

//internal screen functions
void InitializeSynopsis();				//ScreenExit.c
void ScreenCloseAllWindows();           //SCREENCloseAll.c

//PsychGLGlue.c
int		PsychConvertColorToDoubleVector(PsychColorType *color, PsychWindowRecordType *windowRecord, GLdouble *valueArray);
// int		PsychConvertColorAndColorSizeToDoubleVector(PsychColorType *color, int colorSize, GLdouble *valueArray);
void		PsychSetGLColor(PsychColorType *color, PsychWindowRecordType *windowRecord);
void		PsychGLRect(double *psychRect);
char		*PsychGetGLErrorNameString(GLenum errorConstant);
#define		PsychTestForGLErrors()		PsychTestForGLErrorsC(__LINE__, __func__, __FILE__) 
void		PsychTestForGLErrorsC(int lineNum, const char *funcName, const char *fileName);
GLdouble	*PsychExtractQuadVertexFromRect(double *rect, int vertexNumber, GLdouble *vertex);
void		PsychPrepareRenderBatch(PsychWindowRecordType *windowRecord, int coords_pos, int* coords_count, double** xy, int colors_pos, int* colors_count, int* colorcomponent_count, double** colors, unsigned char** bytecolors, int sizes_pos, int* sizes_count, double** size);

// Helper routines for vertically compressed stereo displays: Defined in SCREENSelectStereoDrawBuffer.c
int PsychSwitchCompressedStereoDrawBuffer(PsychWindowRecordType *windowRecord, int newbuffer);
void PsychComposeCompressedStereoBuffer(PsychWindowRecordType *windowRecord);




//functions implementing Screen subcommands. 
PsychError	SCREENNull(void);					
PsychError 	SCREENTestStructures(void);				
PsychError 	MODULEVersion(void);					
PsychError 	SCREENComputer(void);					
PsychError	SCREENScreens(void);					
PsychError 	SCREENPixelSize(void);					 
PsychError 	SCREENPixelSizes(void);					
PsychError 	SCREENNominalFramerate(void);			
PsychError 	SCREENOpenWindow(void);					
PsychError 	SCREENOpenOffscreenWindow(void);		
PsychError 	SCREENClose(void);						
PsychError	SCREENCloseAll(void);				
PsychError 	SCREENCopyWindow(void);				 
PsychError 	SCREENFlip(void);					
PsychError 	SCREENFillRect(void);					
PsychError	SCREENGetImage(void);					
PsychError 	SCREENPutImage(void);					
PsychError 	SCREENHideCursorHelper(void);					
PsychError 	SCREENShowCursorHelper(void);					
PsychError 	SCREENSetMouseHelper(void);				 
PsychError 	SCREENRect(void);					
PsychError 	SCREENWindowScreenNumber(void);			
PsychError 	SCREENWindows(void);					 
PsychError 	SCREENWindowKind(void);					
PsychError 	SCREENIsOffscreen(void);				
PsychError 	SCREENReadNormalizedGammaTable(void);	 
PsychError 	SCREENLoadNormalizedGammaTable(void);	
PsychError 	SCREENglPoint(void);					
PsychError 	SCREENgluDisk(void);					 
PsychError 	SCREENFillOval(void);					
PsychError 	SCREENFrameOval(void) ;					
PsychError 	SCREENTextModes(void);					
PsychError 	SCREENTextMode(void);					
PsychError 	SCREENTextSize(void);					
PsychError 	SCREENTextStyle(void);					
PsychError 	SCREENTextFont(void);					
PsychError      SCREENTextBounds(void);					
PsychError      SCREENDrawText(void);					
PsychError      SCREENTextColor(void);					
PsychError      SCREENPreference(void);					
PsychError      SCREENDrawTexture(void);			
PsychError      SCREENMakeTexture(void);			
PsychError      SCREENFrameRect(void);
PsychError      SCREENDrawLine(void);
PsychError      SCREENFillPoly(void);
PsychError      SCREENFramePoly(void);
PsychError      SCREENGlobalRect(void);
PsychError	SCREENDrawDots(void); 
PsychError	SCREENGetTimeList(void);
PsychError	SCREENClearTimeList(void);
PsychError	SCREENBlendFunction(void);
PsychError      SCREENWindowSize(void); 
PsychError	SCREENTextBackgroundColor(void); 
PsychError      SCREENLineStipple(void);
PsychError      SCREENSelectStereoDrawBuffer(void); 
PsychError      SCREENDrawingFinished(void); 
PsychError      SCREENDrawLines(void);
PsychError      SCREENGetFlipInterval(void);
PsychError      SCREENCloseMovie(void);
PsychError      SCREENOpenMovie(void);
PsychError      SCREENPlayMovie(void);
PsychError      SCREENSetMovieTimeIndex(void);
PsychError      SCREENGetMovieTimeIndex(void);
PsychError      SCREENGetMovieImage(void);
PsychError      SCREENglPushMatrix(void);
PsychError      SCREENglPopMatrix(void);
PsychError      SCREENglLoadIdentity(void);
PsychError      SCREENglTranslate(void);
PsychError      SCREENglScale(void);
PsychError      SCREENglRotate(void);
PsychError      SCREENPreloadTextures(void);
PsychError      SCREENFillArc(void);
PsychError      SCREENDrawArc(void);
PsychError      SCREENFrameArc(void);
PsychError      SCREENWaitBlanking(void);
PsychError      SCREENSetOpenGLTexture(void); 
PsychError      SCREENGetOpenGLTexture(void); 
PsychError      SCREENOpenVideoCapture(void); 
PsychError      SCREENCloseVideoCapture(void); 
PsychError      SCREENStartVideoCapture(void); 
PsychError      SCREENStopVideoCapture(void); 
PsychError      SCREENGetCapturedImage(void); 
PsychError      SCREENSetVideoCaptureParameter(void); 
PsychError      SCREENBeginOpenGL(void);
PsychError      SCREENEndOpenGL(void);
PsychError      SCREENLoadCLUT(void);
PsychError      SCREENDisplaySize(void);
PsychError      SCREENSetOpenGLTextureFromMemPointer(void);
PsychError		SCREENColorRange(void);
PsychError		SCREENHookFunction(void);
PsychError		SCREENOpenProxy(void);
PsychError		SCREENTransformTexture(void);

//experimental
PsychError SCREENGetMouseHelper(void);

//PsychError SCREENSetGLSynchronous(void);		//SCREENSetGLSynchronous.c


//end include once
#endif

