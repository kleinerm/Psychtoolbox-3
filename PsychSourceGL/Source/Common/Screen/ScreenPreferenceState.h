/*
 *    ScreenPreferenceState.h
 *
 *    AUTHORS:
 *
 *    Allen.Ingling@nyu.edu           awi
 *    mario.kleiner.de@gmail.com      mk
 *
 *    PLATFORMS:
 *
 *        All.
 *
 *    HISTORY:
 *
 *        2/28/04  awi        Created.
 *        5/30/05  mk         New preference setting screenSkipSyncTests: If set to TRUE, self-test and calibration will be as
 *                            short and sloppy as possible and some problematic conditions will not cause an abort.
 *                            One can trade-off correctness and accuracy of stimulus presentation against level of annoyance
 *                            during development/debugging of new experiments.
 *        9/30/05  mk         new setting VisualDebugLevel: Defines how much visual feedback PTB should give about errors and
 *                            state: 0=none, 1=only errors, 2=also warnings, 3=also infos, 4=also blue bootup screen, 5=also visual test sheets.
 *        3/7/06   awi        Added state for new preference flag SuppressAllWarnings.
 *
 *    DESCRIPTION:
 *
 *        This file holds state for the Screen Preference command.
 */

#include "Screen.h"

//begin include once 
#ifndef PSYCH_IS_INCLUDED_ScreenPreferenceState
#define PSYCH_IS_INCLUDED_ScreenPreferenceState

//function prototypes

void PrepareScreenPreferences(void);

// Text matching.  Function definition in MiniBox.c 
psych_bool PsychIsPsychMatchCaseSensitive(void);

//PsychTable preferences
int PsychPrefStateGet_PsychTableVersion(void);
const char *PsychPrefStateGet_PsychTableCreator(void);

//Text and Font preferences
int  PsychPrefStateGet_TextYPositionIsBaseline(void);
void PsychPrefStateSet_TextYPositionIsBaseline(int textPosIsBaseline);

int PsychPrefStateGet_TextAntiAliasing(void);
void PsychPrefStateSet_TextAntiAliasing(int mode);

int PsychPrefStateGet_TextRenderer(void);
void PsychPrefStateSet_TextRenderer(int mode);

void PsychPrefStateGet_DefaultFontName(const char **fontName);
void PsychPrefStateSet_DefaultFontName(const char *newName);

int PsychPrefStateGet_DefaultTextSize(void);
void PsychPrefStateSet_DefaultTextSize(int textSize);

int PsychPrefStateGet_DefaultTextStyle(void);
void PsychPrefStateSet_DefaultTextStyle(int textStyle);

psych_bool PsychPrefStateGet_TextAlphaBlending(void);
void PsychPrefStateSet_TextAlphaBlending(psych_bool enableFlag);

// Screen self-test and calibration preferences.  
int PsychPrefStateGet_SkipSyncTests(void);
void PsychPrefStateSet_SkipSyncTests(int level);

int PsychPrefStateGet_VisualDebugLevel(void);
void PsychPrefStateSet_VisualDebugLevel(int level);

int PsychPrefStateGet_ConserveVRAM(void);
void PsychPrefStateSet_ConserveVRAM(int level);

psych_bool PsychPrefStateGet_EmulateOldPTB(void);
void PsychPrefStateSet_EmulateOldPTB(psych_bool level);

// 3D graphics rendering support:
int PsychPrefStateGet_3DGfx(void);
void PsychPrefStateSet_3DGfx(int level);

//Debug preferences.  
psych_bool PsychPrefStateGet_DebugMakeTexture(void);
void PsychPrefStateSet_DebugMakeTexture(psych_bool setFlag);

// Master switch for debug output:
psych_bool PsychPrefStateGet_SuppressAllWarnings(void);
void PsychPrefStateSet_SuppressAllWarnings(psych_bool setFlag);

// Level of verbosity:
int PsychPrefStateGet_Verbosity(void);
void PsychPrefStateSet_Verbosity(int level);

// Master control for method of flip and vbl timestamping:
int PsychPrefStateGet_VBLTimestampingMode(void);
void PsychPrefStateSet_VBLTimestampingMode(int level);

// Override for display endline aka VTOTAL:
int PsychPrefStateGet_VBLEndlineOverride(void);
void PsychPrefStateSet_VBLEndlineOverride(int level);

// Override for display endline multiplicator, relative to VACTIVE:
double PsychPrefStateGet_VBLEndlineMaxFactor(void);
void PsychPrefStateSet_VBLEndlineMaxFactor(double inputDoubleValue);

// Default for selection of video capture engine:
int PsychPrefStateGet_VideoCaptureEngine(void);
void PsychPrefStateSet_VideoCaptureEngine(int mode);

// Window shielding level for windowed mode:
void PsychPrefStateSet_WindowShieldingLevel(int level);
int PsychPrefStateGet_WindowShieldingLevel(void);

// Correction tweak offset for proper Screen('FrameRect') behaviour:
void PsychPrefStateSet_FrameRectCorrection(double level);
double PsychPrefStateGet_FrameRectCorrection(void);

// Tweakable parameters for VBL sync tests and refresh rate calibration:
void PsychPrefStateSet_SynctestThresholds(double maxStddev, int minSamples, double maxDeviation, double maxDuration);
void PsychPrefStateGet_SynctestThresholds(double* maxStddev, int* minSamples, double* maxDeviation, double* maxDuration);

// Shall GStreamer be used instead of Quicktime on 32-bit Windows or OS/X?
void PsychPrefStateSet_UseGStreamer(int value);
int PsychPrefStateGet_UseGStreamer(void);

// Modify/Get screenid -> gpu head mapping:
void PsychPrefStateSet_ScreenToHead(int screenId, int headId, int crtcId, int rankId);
int PsychPrefStateGet_ScreenToHead(int screenId, int rankId);
int PsychPrefStateGet_ScreenToCrtcId(int screenId, int rankId);

//end include once
#endif
