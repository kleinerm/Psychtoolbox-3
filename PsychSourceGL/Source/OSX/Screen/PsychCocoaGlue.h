/*
 
 PsychToolbox3/Source/OSX/Screen/PsychCocoaGlue.h
 
 PLATFORMS:	
 
 This is the OS X 64-Bit Cocoa version only.
 
 TARGETS:
 
 Screen
 
 AUTHORS:
 
 Mario Kleiner       mk      mario.kleiner@tuebingen.mpg.de
 
 DESCRIPTION:
 
 Glue code for window management for the 64-Bit Psychtoolbox,
 using Objective-C wrappers to use Cocoa.
 
 These functions are called by PsychWindowGlue.c. Some are
 specific to Cocoa window management. Some are drop-in replacements
 for missing Carbon 32-Bit functions.
 
*/

#ifndef PSYCH_COCOAGLUE_H
#define PSYCH_COCOAGLUE_H

PsychError PsychCocoaCreateWindow(PsychWindowRecordType *windowRecord,
                                  PsychRectType      screenRect,
                                  const Rect *       contentBounds,
                                  WindowClass        wclass,
                                  WindowAttributes   addAttribs,
                                  WindowRef *        outWindow);

WindowRef GetUserFocusWindow(void);

void PsychCocoaShowWindow(WindowRef window);

CGLContextObj PsychGetCocoaOpenGLContext(WindowRef window);

psych_bool PsychCocoaSetupAndAssignOpenGLContextsFromCGLContexts(WindowRef window, PsychWindowRecordType *windowRecord);

#endif
