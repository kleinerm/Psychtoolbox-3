/*
	SCREENPreference.c		
  
	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:
	
		Mac OS X MATLAB only.
    
	HISTORY:
  
		2/28/04		awi		Created.
		10/29/04	awi		Fixed two warnings, one where a const char should have been been char, the other the other way around.
		1/20/05		awi		Added a preference for debugging MakeTexture 
		5/30/05		mk		New preference setting screenSkipSyncTests -- allow skipping/shortening of some internal tests.
		5/30/05		mk		New preference setting screenVisualDebugLevel.
		3/07/05		awi		New preference SuppressAllWarnings.
                11/15/06        mk              New preference vbl & flip timestamping mode.
 
	DESCRIPTION:
  
		Read and set preferences.
  
		Note that we do don't do sanity checking on arguments provided in conjunction for unsuported preferences.  That might be conveivably be a mildly
		useful feature for catching cross-platform errors if we detect argument type errors before detecting the error of using a platform unsupported preference. 
  
*/


#include "Screen.h"

#if PSYCH_SYSTEM != PSYCH_WINDOWS

// for getpid()
#include <sys/types.h>
#include <unistd.h>

#else

// Windows doesn't have getpid() - We create a pseudo-getpid:
psych_uint64 getpid(void)
{
  HANDLE p;
  p=GetCurrentProcess();
  return((psych_uint64) p);
}
#endif

/*
OS 9 Psychtoolbox preferences:

	Supported
oldBool=Screen('Preference', 'IgnoreCase' [,bool])
tick0Secs=Screen('Preference', 'Tick0Secs', tick0Secs)
psychTableVersion=Screen('Preference', 'PsychTableVersion')
mexFunctionName=Screen('Preference', 'PsychTableCreator')
proc=Screen('Preference', 'Process', signature)   //returns PID and does not accept the signature 


	Read supported but will not accept set:
oldBool=Screen('Preference','Backgrounding',[bool])
oldSecondsMultiplier=Screen('Preference', 'SecondsMultiplier' [,secondsMultiplier])
	
	Unsupported and we should
oldBool=Screen('Preference' ',AllowMirroring' [,bool])
bool=Screen('Preference' ',MirroringIsOn')
oldBits=Screen(windowPtrOrScreenNumber, 'Preference', 'DacBits', bits) 

	Unsupported
available=Screen('Preference','Available')
nextProc=Screen('Preference','NextProcess',[proc])
oldBool=Screen('Preference','UseNewHideMenuBar',[bool])

	Unsupported OS 9 Mac video driver specific 
bool=Screen(windowPtrOrScreenNumber,'Preference','FixedClut')
oldBool=Screen(windowPtrOrScreenNumber,'Preference','SetClutDriverWaitsForBlanking',[bool])
oldBool=Screen(windowPtrOrScreenNumber,'Preference','AskSetClutDriverToWaitForBlanking',[bool])
oldValue=Screen(windowPtrOrScreenNumber,'Preference','SetClutSAI',[value])
oldBool=Screen(windowPtrOrScreenNumber,'Preference','SetClutCallsWaitBlanking',[bool])
oldBool=Screen(windowPtrOrScreenNumber,'Preference','SetClutPunchesBlankingClock',[bool])
oldBool=Screen(windowPtrOrScreenNumber,'Preference','InterruptPunchesBlankingClock',[bool])
oldPriority=Screen(windowPtrOrScreenNumber,'Preference','MaxPriorityForBlankingInterrupt',[priority])
oldBool=Screen(windowPtrOrScreenNumber,'Preference','WaitBlankingAlwaysCallsSetClut',[bool])
oldSecs=Screen(windowPtrOrScreenNumber,'Preference','BlankingDuration',[secs])
oldN=Screen(windowPtrOrScreenNumber,'Preference','MinimumEntriesForSetClutToWaitForBlanking',[n])
oldPriority=Screen(windowPtrOrScreenNumber,'Preference','MinimumSetClutPriority',[priority])
oldPriority=Screen(windowPtrOrScreenNumber,'Preference','MaximumSetClutPriority',[priority])
oldBool=Screen(windowPtrOrScreenNumber,'Preference','DipPriorityAfterSetClut',[bool])
oldBool=Screen(windowPtrOrScreenNumber,'Preference','UsesHighGammaBits',[bool])

*/

static char useString[] = "oldPreferenceValue = Screen('Preference', preferenceName, [newPreferenceValue])";
//                                                         0             1                2
static char synopsisString[] = 
	"Get or set a Psychtoolbox preference."
	"\noldBool = Screen('Preference', 'IgnoreCase', [bool]);"
	"\ntick0Secs = Screen('Preference', 'Tick0Secs', tick0Secs);"
	"\npsychTableVersion = Screen('Preference', 'PsychTableVersion');"
	"\nmexFunctionName = Screen('Preference', 'PsychTableCreator');"
	"\nproc = Screen('Preference', 'Process', signature);"
	"\nproc = Screen('Preference', 'DebugMakeTexture', enableDebugging);"
	"\noldEnableFlag = Screen('Preference', 'TextAlphaBlending', [enableFlag]);"
	"\noldEnableFlag = Screen('Preference', 'DefaultTextYPositionIsBaseline', [enableFlag]);"
	"\noldEnableFlag = Screen('Preference', 'SkipSyncTests', [enableFlag]);"
	"\noldLevel = Screen('Preference', 'VisualDebugLevel', level);"
	"\noldMode = Screen('Preference', 'ConserveVRAM', mode);"
	"\nActivate compatibility mode: Try to behave like the old MacOS-9 Psychtoolbox:"
	"\noldEnableFlag = Screen('Preference', 'EmulateOldPTB', [enableFlag]);"
	"\noldEnableFlag = Screen('Preference', 'Enable3DGraphics', [enableFlag]);"
	"\noldEnableFlag = Screen('Preference', 'SuppressAllWarnings', [enableFlag]);"
	"\noldMode = Screen('Preference', 'VBLTimestampingMode', [newmode]);"
	"\noldLevel = Screen('Preference', 'Verbosity' [,level]);";
			
static char seeAlsoString[] = "";	


#define kPsychNumUnsupportedMacVideoPreferences			14
static char *unsupportedMacVideoPreferenceNames[]={
	"FixedClut" 
	"SetClutDriverWaitsForBlanking",
	"AskSetClutDriverToWaitForBlanking",
	"SetClutSAI SetClutCallsWaitBlanking",
	"SetClutPunchesBlankingClock",
	"InterruptPunchesBlankingClock",
	"MaxPriorityForBlankingInterrupt",
	"WaitBlankingAlwaysCallsSetClut",
	"BlankingDuration",
	"MinimumEntriesForSetClutToWaitForBlanking",
	"MinimumSetClutPriority",
	"MaximumSetClutPriority",
	"DipPriorityAfterSetClut",
	"UsesHighGammaBits"
};

#define kPsychNumUnsupportedMacNonVideoPreferences			6
static char *unsupportedMacNonVideoPreferenceNames[]={
	"AllowMirroring",
	"MirroringIsOn",
	"DacBits",
	"Available",
	"NextProcess",
	"UseNewHideMenuBar"
};

PsychError SCREENPreference(void)  
{

	PsychArgFormatType		arg1Type;
	char					*preferenceName, *newFontName;
	const char				*tableCreator, *oldDefaultFontName;
	Boolean					preferenceNameArgumentValid, booleanInput, ignoreCase, tempFlag, textAlphaBlendingFlag, suppressAllWarningsFlag;
	int						numInputArgs, i, newFontStyleNumber, newFontSize, tempInt;
	double					returnDoubleValue, inputDoubleValue;
	
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString,seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous or missing arguments
	PsychErrorExit(PsychCapNumInputArgs(3));			
	PsychErrorExit(PsychRequireNumInputArgs(1));		   
	PsychErrorExit(PsychCapNumOutputArgs(1));
	
	numInputArgs=PsychGetNumInputArgs();
	arg1Type=PsychGetArgType(1);
	preferenceNameArgumentValid=FALSE;
	
	//Cases which require both a window pointer or screen number and preference name.  Argument 1 is the wposn and argument 2 is the preference name.
	if( numInputArgs >= 2 && (PsychIsScreenNumberArg(1) || PsychIsScreenNumberArg(1)) && PsychGetArgType(2)==PsychArgType_char ){
		PsychAllocInCharArg(2, kPsychArgRequired, &preferenceName);
		//preferences which require window pointer or screen number argument which we DO NOT support  
		for(i=0;i<kPsychNumUnsupportedMacVideoPreferences;i++){
			if(PsychMatch(preferenceName, unsupportedMacVideoPreferenceNames[i]))
				PsychErrorExit(PsychError_unsupportedOS9Preference);
		}
		//insert here conditionals  to act on prefernces which accept a window pointer or screen number argument which we DO support.
		PsychErrorExit(PsychError_unrecognizedPreferenceName);
	}

	//Cases which do not require a wposn.  Argument 1 is the preference name.  if present Argument 2 is the new value
	if(arg1Type==PsychArgType_char){
		PsychAllocInCharArg(1, kPsychArgRequired, &preferenceName);
		//Preferernces which we do not support and which do not require a wposn
		for(i=0;i<kPsychNumUnsupportedMacNonVideoPreferences;i++){
			if(PsychMatch(preferenceName, unsupportedMacNonVideoPreferenceNames[i]))
				PsychErrorExit(PsychError_unsupportedOS9Preference);
		}
		//Preferences which we do support
		if(PsychMatch(preferenceName, "IgnoreCase")){
			ignoreCase=!PsychIsPsychMatchCaseSensitive();
			PsychCopyOutFlagArg(1, kPsychArgOptional, ignoreCase);
			if(numInputArgs==2){
				PsychCopyInFlagArg(2, kPsychArgRequired, &booleanInput);
				PsychSetPsychMatchCaseSenstive(!booleanInput);			
			}
			preferenceNameArgumentValid=TRUE;
		}else 
		if(PsychMatch(preferenceName, "Tick0Secs")){
			if(PsychCopyInDoubleArg(2, kPsychArgOptional, &inputDoubleValue) && inputDoubleValue==PsychGetNanValue())
				PsychEstimateGetSecsValueAtTickCountZero();
			returnDoubleValue=PsychGetEstimatedSecsValueAtTickCountZero();
			PsychCopyOutDoubleArg(1, kPsychArgOptional, returnDoubleValue);
			preferenceNameArgumentValid=TRUE;
		}else 
		if(PsychMatch(preferenceName, "PsychTableVersion")){
			if(numInputArgs==2)
				PsychErrorExit(PsychError_extraInputArg);
			PsychCopyOutDoubleArg(1, kPsychArgOptional, (double)PsychPrefStateGet_PsychTableVersion());
			preferenceNameArgumentValid=TRUE;
		}else 
		if(PsychMatch(preferenceName, "PsychTableCreator")){
			if(numInputArgs==2)
				PsychErrorExit(PsychError_extraInputArg);
			tableCreator=PsychPrefStateGet_PsychTableCreator();
			PsychCopyOutCharArg(1, kPsychArgOptional, tableCreator);
			preferenceNameArgumentValid=TRUE;
		}else 
		if(PsychMatch(preferenceName, "Process")){
			if(numInputArgs==2)
				PsychErrorExit(PsychError_extraInputArg);
			PsychCopyOutDoubleArg(1, kPsychArgOptional, (double)getpid());
			preferenceNameArgumentValid=TRUE;


		}else 
		if(PsychMatch(preferenceName, "DefaultFontName")){
			PsychPrefStateGet_DefaultFontName(&oldDefaultFontName);
			PsychCopyOutCharArg(1, kPsychArgOptional, oldDefaultFontName);
			if(numInputArgs==2){
				PsychAllocInCharArg(2, kPsychArgRequired, &newFontName);
				PsychPrefStateSet_DefaultFontName(newFontName);
			}
			preferenceNameArgumentValid=TRUE;
			
		}else 
		if(PsychMatch(preferenceName, "DefaultFontStyle")){
			PsychCopyOutDoubleArg(1, kPsychArgOptional, PsychPrefStateGet_DefaultTextStyle());
			if(numInputArgs==2){
				PsychCopyInIntegerArg(2, kPsychArgRequired, &newFontStyleNumber);
				PsychPrefStateSet_DefaultTextStyle(newFontStyleNumber);
			}
			preferenceNameArgumentValid=TRUE;
		}else
		if(PsychMatch(preferenceName, "DefaultTextYPositionIsBaseline")){
			PsychCopyOutDoubleArg(1, kPsychArgOptional, PsychPrefStateGet_TextYPositionIsBaseline());
			if(numInputArgs==2){
				PsychCopyInIntegerArg(2, kPsychArgRequired, &tempInt);
				PsychPrefStateSet_TextYPositionIsBaseline(tempInt);
			}
			preferenceNameArgumentValid=TRUE;
		}else 
		if(PsychMatch(preferenceName, "DefaultFontSize")){
			PsychCopyOutDoubleArg(1, kPsychArgOptional, PsychPrefStateGet_DefaultTextSize());
			if(numInputArgs==2){
				PsychCopyInIntegerArg(2, kPsychArgRequired, &newFontSize);
				PsychPrefStateSet_DefaultTextSize(newFontSize);
			}
			preferenceNameArgumentValid=TRUE;
		}else 
		if(PsychMatch(preferenceName, "DebugMakeTexture")){
			PsychCopyOutDoubleArg(1, kPsychArgOptional, PsychPrefStateGet_DebugMakeTexture());
			if(numInputArgs==2){
				PsychCopyInFlagArg(2, kPsychArgRequired, &tempFlag);
				PsychPrefStateSet_DebugMakeTexture(tempFlag);
			}
			preferenceNameArgumentValid=TRUE;
		}else 
			if(PsychMatch(preferenceName, "SkipSyncTests")){
			PsychCopyOutDoubleArg(1, kPsychArgOptional, PsychPrefStateGet_SkipSyncTests());
			if(numInputArgs==2){
				PsychCopyInIntegerArg(2, kPsychArgRequired, &tempInt);
				PsychPrefStateSet_SkipSyncTests(tempInt);
			}
			preferenceNameArgumentValid=TRUE;
		}else 
			if(PsychMatch(preferenceName, "VisualDebugLevel")){
			PsychCopyOutDoubleArg(1, kPsychArgOptional, PsychPrefStateGet_VisualDebugLevel());
			if(numInputArgs==2){
				PsychCopyInIntegerArg(2, kPsychArgRequired, &tempInt);
				PsychPrefStateSet_VisualDebugLevel(tempInt);
			}
			preferenceNameArgumentValid=TRUE;
		}else 
			if(PsychMatch(preferenceName, "VBLTimestampingMode")){
			PsychCopyOutDoubleArg(1, kPsychArgOptional, PsychPrefStateGet_VBLTimestampingMode());
			if(numInputArgs==2){
                            PsychCopyInIntegerArg(2, kPsychArgRequired, &tempInt);
                            PsychPrefStateSet_VBLTimestampingMode(tempInt);
			}
			preferenceNameArgumentValid=TRUE;
		}else 
			if(PsychMatch(preferenceName, "ConserveVRAM")){
					PsychCopyOutDoubleArg(1, kPsychArgOptional, PsychPrefStateGet_ConserveVRAM());
					if(numInputArgs==2){
						PsychCopyInIntegerArg(2, kPsychArgRequired, &tempInt);
						PsychPrefStateSet_ConserveVRAM(tempInt);
					}
			preferenceNameArgumentValid=TRUE;
		}else 
			if(PsychMatch(preferenceName, "Verbosity")){
					PsychCopyOutDoubleArg(1, kPsychArgOptional, PsychPrefStateGet_Verbosity());
					if(numInputArgs==2){
						PsychCopyInIntegerArg(2, kPsychArgRequired, &tempInt);
						PsychPrefStateSet_Verbosity(tempInt);
					}
			preferenceNameArgumentValid=TRUE;
		}else 
			if(PsychMatch(preferenceName, "EmulateOldPTB")){
				PsychCopyOutDoubleArg(1, kPsychArgOptional, PsychPrefStateGet_EmulateOldPTB());
				if(numInputArgs==2){
					PsychCopyInFlagArg(2, kPsychArgRequired, &tempFlag);
					PsychPrefStateSet_EmulateOldPTB(tempFlag);
				}
				preferenceNameArgumentValid=TRUE;
		}else 
			if(PsychMatch(preferenceName, "Enable3DGraphics")){
				PsychCopyOutDoubleArg(1, kPsychArgOptional, PsychPrefStateGet_3DGfx());
				if(numInputArgs==2){
					PsychCopyInFlagArg(2, kPsychArgRequired, &tempFlag);
					PsychPrefStateSet_3DGfx(tempFlag);
				}
				preferenceNameArgumentValid=TRUE;
		}else 
			if(PsychMatch(preferenceName, "TextAlphaBlending")){
				textAlphaBlendingFlag=PsychPrefStateGet_TextAlphaBlending();
				PsychCopyOutFlagArg(1, kPsychArgOptional, textAlphaBlendingFlag);
				if(numInputArgs==2){
					PsychCopyInFlagArg(2, kPsychArgRequired, &booleanInput);
					PsychPrefStateSet_TextAlphaBlending(booleanInput);
				}
				preferenceNameArgumentValid=TRUE;
		}else 
			if(PsychMatch(preferenceName, "SuppressAllWarnings")){
				suppressAllWarningsFlag=PsychPrefStateGet_SuppressAllWarnings();
				PsychCopyOutFlagArg(1, kPsychArgOptional, suppressAllWarningsFlag);
				if(numInputArgs==2){
					PsychCopyInFlagArg(2, kPsychArgRequired, &booleanInput);
					PsychPrefStateSet_SuppressAllWarnings(booleanInput);
				}
				preferenceNameArgumentValid=TRUE;
		}else 
			PsychErrorExit(PsychError_unrecognizedPreferenceName);
	}
	
	if(!preferenceNameArgumentValid)
		PsychErrorExitMsg(PsychError_user, "Invalid arguments to preferences command");
		
	return(PsychError_none);
}




