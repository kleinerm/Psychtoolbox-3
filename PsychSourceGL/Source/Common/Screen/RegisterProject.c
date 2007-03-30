/*
	PsychToolbox2/Source/Common/Screen/RegisterProject.c		
  
	PROJECTS: 
  
		Screen only.  
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
  
		Only OS X for now
    
	HISTORY:

		8/20/02  awi		Created. 
 
	DESCRIPTION: 
  
	  The psychtoolbox is seperated into two layers:
	  
	  -A lower layer which includes C files and all headers included 
	  by Psych.h.  All Psychtoolbox projects should include Psych.h
	  
	  -An upper layer which includes all C files and headers specific 
	  to the project, for example only the Screen project includes 
	  Screen.h, which  includes among other things declarations for 
	  Screen.h functions. 
	  
	  That the entry point (e.g. MexFunction) for the module is in the
	  lower Psych.h layer breaks the typical inclusion heirarchy where the 
	  library headers are included in the project headers but not vice
	  versa. So with Psychtoolbox project the library must include 
	  the project headers and to do this cleanly and portably we allow
	  the lower layer to contain only one header file from the 
	  upper layer which is alwyas named RegisterProject.h, which in turn 
	  includes the the header for the project (e.g. Screen.h).  This 
	  avoids the messiness of having to name project-specific headers 
	  for every project within a portable library; The library would 
	  would have to be modified to be used in a project.
		
	  Every psychtoolbox project must contain the RegisterProject.h  
	  header file, and that file should #include the project header file,
	  (e.g. Screen.h).  
	  
	  Every psychtoolbox project should include the function 
	  PsychModuleInit() which is called when the compiled module is loaded: 
	  PsychModuleInit should do whatever initilization is the project requires
	  and register subfunctions.  The Psychtoolbox provides two functions 
	  for registering projects: 
	  
	  PsychError PsychRegisterProjectExit(PsychExitFunctionType *exitFunc)
	  
		-Registers a cleanup function which is invoked before the module 
		is purged.
		
	  PsychError PsychRegisterProjectFunction(char *name,  func)
	  
		-When called with the name argument and NULL for the function 
		argument it registers name as the project name.
		
		-When called with name and a non-null func value registers 
		func as to be invoked if the module is called with the name as
		the first or second argument.
		
		-when called with NULL for name and a non-null for func
		registers a module function to be called when the module is
		invoked with no subcommand name.  The resulting behavior varies
		depeinding on whether the project uses named subfunctions.  
		The psychoolbox operates in either of two ways depending on 
		whether named subfunctions are used.  
		
			-If ONLY the null name function is regsitered then the library
			operates in "direct mode" where control passes to the function
			passed in the func argument whenever the module is invoked.
			
			-If any named subfunction is registered then the function 
			registered with the NULL name is invoked whenever the module
			is invoked with no arguments.  
			
	TO DO:
	
		The DESCRIPTION section above should be moved to a separate help document.  

*/

//begin include once 


#include "RegisterProject.h"


// PsychModuleInit is in Screen.cpp
// PsychProjectExit is in Screen.cpp
// PsychProjectSelectFunction is in Screen.cpp


PsychError PsychModuleInit(void)
{
        //register the project exit function
        PsychErrorExitMsg(PsychRegisterExit(&ScreenExitFunction), "Failed to register the Screen exit function.");
	
	//register the project function which is called when the module
	//is invoked with no arguments:
	PsychErrorExitMsg(PsychRegister(NULL,  &PsychDisplayScreenSynopsis), "Failed to register the Screen synopsis function.");

	//register the module name
 	PsychErrorExitMsg(PsychRegister("Screen", NULL), "Failed to register the Screen module name.");

	//register named subfunctions
	
	//enable for debugging purposes
	PsychErrorExit(PsychRegister("Null",  &SCREENNull));
//	PsychErrorExit(PsychRegister("TestStructures",  &SCREENTestStructures));
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));
	PsychErrorExit(PsychRegister("Computer",  &SCREENComputer));
	PsychErrorExit(PsychRegister("Screens", &SCREENScreens));
	PsychErrorExit(PsychRegister("PixelSize",&SCREENPixelSize));
	PsychErrorExit(PsychRegister("PixelSizes",&SCREENPixelSizes));
	PsychErrorExit(PsychRegister("OpenWindow",  &SCREENOpenWindow));
	PsychErrorExit(PsychRegister("OpenOffscreenWindow",  &SCREENOpenOffscreenWindow));
	PsychErrorExit(PsychRegister("Close",  &SCREENClose));
	PsychErrorExit(PsychRegister("CloseAll",  &SCREENCloseAll)); 
	PsychErrorExit(PsychRegister("Flip", &SCREENFlip));
	PsychErrorExit(PsychRegister("FillRect", &SCREENFillRect));
	PsychErrorExit(PsychRegister("GetImage", &SCREENGetImage));
	PsychErrorExit(PsychRegister("PutImage", &SCREENPutImage));
	PsychErrorExit(PsychRegister("HideCursorHelper", &SCREENHideCursorHelper));
	PsychErrorExit(PsychRegister("ShowCursorHelper", &SCREENShowCursorHelper));
	PsychErrorExit(PsychRegister("SetMouseHelper", &SCREENSetMouseHelper)); 
	//PsychErrorExit(PsychRegister("SetMouse", &SCREENPositionCursor));
	PsychErrorExit(PsychRegister("Rect", &SCREENRect));
	PsychErrorExit(PsychRegister("WindowScreenNumber", &SCREENWindowScreenNumber));
	PsychErrorExit(PsychRegister("Windows", &SCREENWindows));
	PsychErrorExit(PsychRegister("WindowKind", &SCREENWindowKind));
	PsychErrorExit(PsychRegister("IsOffscreen", &SCREENIsOffscreen));
	PsychErrorExit(PsychRegister("ReadNormalizedGammaTable", &SCREENReadNormalizedGammaTable));
	PsychErrorExit(PsychRegister("LoadNormalizedGammaTable", &SCREENLoadNormalizedGammaTable));
	PsychErrorExit(PsychRegister("FrameRate", &SCREENNominalFramerate));
	PsychErrorExit(PsychRegister("NominalFrameRate", &SCREENNominalFramerate));
	PsychErrorExit(PsychRegister("glPoint", &SCREENglPoint));
	PsychErrorExit(PsychRegister("gluDisk", &SCREENgluDisk));
	PsychErrorExit(PsychRegister("FillOval", &SCREENFillOval));
	PsychErrorExit(PsychRegister("FrameOval", &SCREENFrameOval));
	PsychErrorExit(PsychRegister("TextModes", &SCREENTextModes));
	PsychErrorExit(PsychRegister("TextMode", &SCREENTextMode));
	PsychErrorExit(PsychRegister("TextSize", &SCREENTextSize));
	PsychErrorExit(PsychRegister("TextStyle", &SCREENTextStyle));
	PsychErrorExit(PsychRegister("TextFont", &SCREENTextFont));
	PsychErrorExit(PsychRegister("TextBounds", &SCREENTextBounds));
	PsychErrorExit(PsychRegister("DrawText", &SCREENDrawText));
	PsychErrorExit(PsychRegister("TextColor", &SCREENTextColor));
	PsychErrorExit(PsychRegister("Preference", &SCREENPreference));
	PsychErrorExit(PsychRegister("MakeTexture", &SCREENMakeTexture));
	PsychErrorExit(PsychRegister("DrawTexture", &SCREENDrawTexture));
	PsychErrorExit(PsychRegister("FrameRect", &SCREENFrameRect));
	PsychErrorExit(PsychRegister("DrawLine", &SCREENDrawLine));
	PsychErrorExit(PsychRegister("FillPoly", &SCREENFillPoly));
	PsychErrorExit(PsychRegister("FramePoly", &SCREENFramePoly));
	PsychErrorExit(PsychRegister("GlobalRect", &SCREENGlobalRect));
	PsychErrorExit(PsychRegister("DrawDots", &SCREENDrawDots));
	PsychErrorExit(PsychRegister("GetTimeList", &SCREENGetTimeList));
	PsychErrorExit(PsychRegister("ClearTimeList", &SCREENClearTimeList));
	PsychErrorExit(PsychRegister("BlendFunction", &SCREENBlendFunction));
	PsychErrorExit(PsychRegister("WindowSize", &SCREENWindowSize));
	PsychErrorExit(PsychRegister("GetMouseHelper", &SCREENGetMouseHelper));
	PsychErrorExit(PsychRegister("TextBackgroundColor", &SCREENTextBackgroundColor));
	PsychErrorExit(PsychRegister("LineStipple", &SCREENLineStipple));  
	PsychErrorExit(PsychRegister("SelectStereoDrawBuffer", &SCREENSelectStereoDrawBuffer));
	PsychErrorExit(PsychRegister("DrawingFinished", &SCREENDrawingFinished));
	PsychErrorExit(PsychRegister("DrawLines", &SCREENDrawLines));
	PsychErrorExit(PsychRegister("GetFlipInterval", &SCREENGetFlipInterval));
	PsychErrorExit(PsychRegister("CloseMovie", &SCREENCloseMovie));
	PsychErrorExit(PsychRegister("OpenMovie", &SCREENOpenMovie));
	PsychErrorExit(PsychRegister("PlayMovie", &SCREENPlayMovie));
	PsychErrorExit(PsychRegister("SetMovieTimeIndex", &SCREENSetMovieTimeIndex));
	PsychErrorExit(PsychRegister("GetMovieTimeIndex", &SCREENGetMovieTimeIndex));
	PsychErrorExit(PsychRegister("GetMovieImage", &SCREENGetMovieImage));
	PsychErrorExit(PsychRegister("glPushMatrix", &SCREENglPushMatrix));
	PsychErrorExit(PsychRegister("glPopMatrix", &SCREENglPopMatrix));
	PsychErrorExit(PsychRegister("glLoadIdentity", &SCREENglLoadIdentity));
	PsychErrorExit(PsychRegister("glTranslate", &SCREENglTranslate));
	PsychErrorExit(PsychRegister("glScale", &SCREENglScale));
	PsychErrorExit(PsychRegister("glRotate", &SCREENglRotate));
	PsychErrorExit(PsychRegister("PreloadTextures", &SCREENPreloadTextures));
	PsychErrorExit(PsychRegister("FillArc", &SCREENFillArc));
	PsychErrorExit(PsychRegister("DrawArc", &SCREENDrawArc));
	PsychErrorExit(PsychRegister("FrameArc", &SCREENFrameArc));
	PsychErrorExit(PsychRegister("CopyWindow", &SCREENCopyWindow));
	PsychErrorExit(PsychRegister("WaitBlanking", &SCREENWaitBlanking));
	PsychErrorExit(PsychRegister("GetOpenGLTexture", &SCREENGetOpenGLTexture));
	PsychErrorExit(PsychRegister("SetOpenGLTexture", &SCREENSetOpenGLTexture));
	PsychErrorExit(PsychRegister("BeginOpenGL", &SCREENBeginOpenGL));
	PsychErrorExit(PsychRegister("EndOpenGL", &SCREENEndOpenGL));
	PsychErrorExit(PsychRegister("OpenVideoCapture", &SCREENOpenVideoCapture));
	PsychErrorExit(PsychRegister("CloseVideoCapture", &SCREENCloseVideoCapture));
	PsychErrorExit(PsychRegister("StartVideoCapture", &SCREENStartVideoCapture));
	PsychErrorExit(PsychRegister("StopVideoCapture", &SCREENStopVideoCapture));
	PsychErrorExit(PsychRegister("GetCapturedImage", &SCREENGetCapturedImage));
	PsychErrorExit(PsychRegister("SetVideoCaptureParameter", &SCREENSetVideoCaptureParameter));
	PsychErrorExit(PsychRegister("LoadCLUT", &SCREENLoadCLUT));
	PsychErrorExit(PsychRegister("DisplaySize", &SCREENDisplaySize));
	PsychErrorExit(PsychRegister("SetOpenGLTextureFromMemPointer", &SCREENSetOpenGLTextureFromMemPointer));
	PsychErrorExit(PsychRegister("ColorRange", &SCREENColorRange));
	PsychErrorExit(PsychRegister("HookFunction", &SCREENHookFunction));
 	PsychErrorExit(PsychRegister("OpenProxy", &SCREENOpenProxy));
	PsychErrorExit(PsychRegister("TransformTexture", &SCREENTransformTexture));
	//Experiments


	//PsychErrorExit(PsychRegister("SetGLSynchronous", &SCREENSetGLSynchronous));
	//InitializeVideoSubsystem();  //SDL_Init or whatever.  Keep this because we might need it for theh Windows implementation.  
	
	PsychSetModuleAuthorByInitials("awi");
	PsychSetModuleAuthorByInitials("dhb");
	PsychSetModuleAuthorByInitials("dgp");
	PsychSetModuleAuthorByInitials("kas");
	PsychSetModuleAuthorByInitials("fjc");
	PsychSetModuleAuthorByInitials("mk");
	PsychSetModuleAuthorByInitials("cb");

	InitializeSynopsis();
	InitializePsychDisplayGlue();
	InitWindowBank();
	PsychMovieInit();
	PsychVideoCaptureInit();
        
	PrepareScreenPreferences();
    
	// Reset the "userspaceGL" flag which tells PTB that userspace GL rendering was active
	// due to Screen('BeginOpenGL') command.
	PsychSetUserspaceGLFlag(FALSE);

	return(PsychError_none);

}




