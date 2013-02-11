/*
  PsychToolbox3/Source/Linux/Screen/PsychWindowGlueWaffle.h
    
  PLATFORMS:  
    
  This is the Linux/Waffle version only.
                
  AUTHORS:
    
  Mario Kleiner           mk              mario.kleiner at tuebingen.mpg.de

  HISTORY:

  2/12/13            mk    Created - Derived from Linux/X11-GLX version.    

  DESCRIPTION:
    
  Functions in this file comprise an abstraction layer for probing and controlling window state, except for window content.  
        
  Each C function which implements a particular Screen subcommand should be platform neutral.  For example, the source to SCREENPixelSizes() 
  should be platform-neutral, despite that the calls in OS X and Linux to detect available pixel sizes are different.  The platform 
  specificity is abstracted out in C files which end it "Glue", for example PsychScreenGlue, PsychWindowGlue, PsychWindowTextClue.

  NOTES:
    
  TO DO: 
     
*/

//include once
#ifndef PSYCH_IS_INCLUDED_PsychWindowGlueWaffle
#define PSYCH_IS_INCLUDED_PsychWindowGlueWaffle

#include "Screen.h"

psych_bool PsychOSOpenOnscreenWindowWaffle(PsychScreenSettingsType * screenSettings, PsychWindowRecordType * windowRecord, int numBuffers, int stereomode, int conserveVRAM);
psych_bool PsychOSCloseWindowWaffle(PsychWindowRecordType * windowRecord);
psych_bool PsychOSFlipWindowBuffersWaffle(PsychWindowRecordType * windowRecord);
psych_bool PsychOSSetGLContextWaffle(PsychWindowRecordType * windowRecord);
psych_bool PsychOSUnsetGLContextWaffle(PsychWindowRecordType * windowRecord);
psych_bool PsychOSSetUserGLContextWaffle(PsychWindowRecordType * windowRecord, psych_bool copyfromPTBContext);

//end include once
#endif
