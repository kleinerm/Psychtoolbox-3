/*
	PsychSourceGL/Source/Common/CocoaEventBridge/COCOAEVENTBRIDGEGetChar.c		
  
	PROJECTS: 
  
		CocoaEventBridge only
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
  
		Only OS X.
    
	HISTORY:

		9/14/05		awi		Wrote it .


*/

#include "EventBridgeBundleHeader.h"
//#include "CocoaEventBridgeHelpers.h"
#include "CocoaEventBridge.h"

static char useString[] = "[char, when]=CocoaEventBridge('GetChar');";
static char synopsisString[] = 
        "Wait (if necessary) for a typed character and return it."
		"\n\n"
		"Do not call CocoaEventBridge('GetChar') directly.  Instead, use the Psychtoolbox function \"GetChar\".";
static char seeAlsoString[] = "GetChar";	

PsychError COCOAEVENTBRIDGEGetChar(void) 
{
	static psych_bool				firstTime=TRUE;
    double						returnValue, inputValue;
	static	psych_bool wasWindowOpenedFlag=FALSE;
	psych_bool						isThere, inputValueBoolean, lostKeyWindowFlag;
//	InitializeCocoaProc			CocoaInitializeCocoa;
	char						readChar[2];
	double						readTime;
	int							numKeypresses, numOutputArgs;
	CFDictionaryRef				keypressDictionary=NULL, keypressModifierFlags=NULL;
	CFStringRef					keypressCharacter=NULL;
	CFNumberRef					keypressTime=NULL, keypressAddress=NULL, keypressTickCount=NULL;
	double						keypressTimeDouble, keypressTickCountDouble;
	char						keypressCharacterUTF8[2];
	CFRange						characterRange;
	UniChar						keypressCharacterUnicode[1];
	double						keypressCharacterAsValue, nowGetSecs, nowTickCount, characterTickCount, keypressAddressDouble;
	psych_bool						loadBundleError;
	//for the return structure in the second argument
	const char *charTimeFieldNames[]={"ticks", "secs", "address", "alphaLock", "commandKey", "controlKey", "optionKey", "shiftKey", "numericKeypad", "functionKey"};
    int 	numStructElements=1, numStructFieldNames=10;
	PsychGenericScriptType	*charTimeStruct;
	CFNumberRef					alphaLock, commandKey, controlKey, optionKey, shiftKey, numericKeypad, helpKey, functionKey;
	char						alphaLockCFlag, commandKeyCFlag, controlKeyCFlag, optionKeyCFlag, shiftKeyCFlag, numericKeypadCFlag, helpKeyCFlag, functionKeyCFlag;
	

	//all subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(1));

	loadBundleError=LoadCocoaBundle();
	if(loadBundleError)
		PsychErrorExitMsg(PsychError_internal, "Failed to load the cocoa bundle.");
		
    
	//Open the window.  OpenGetCharWindow() and MakeGetCharWindowInvisible() only act once if called repeatedly.
	OpenGetCharWindow();
	#ifndef DEBUG_USE_VISIBLE_WINDOW
	MakeGetCharWindowInvisible();
	#endif

	StartKeyGathering();
	MakeKeyWindow();
	lostKeyWindowFlag=FALSE;
	while(GetNumKeypresses() < 1){
		PsychWaitIntervalSeconds((double)0.005);
		//this would become an infinite loop if the user moves focus to another window, because our key collection window would never
		//receive input.  Therefore we detect if our window loses keyWindow status.  We could be more forceful about this and bring the 
		//window back into focus, if we have access to NSApplication within our Cocoa bundle.  
		lostKeyWindowFlag=!IsKeyWindow();
		if(lostKeyWindowFlag)
			break;
	}
	
//	StopKeyGathering();
//	if(!lostKeyWindowFlag)
//		RevertKeyWindow();	//restores the key window to what it was before we took it.
    if(!lostKeyWindowFlag){
		keypressDictionary=(CFDictionaryRef)CopyReadNextKeypress();
		if(keypressDictionary != NULL){			
			keypressCharacter=CFDictionaryGetValue(keypressDictionary, CFSTR("character"));
			keypressTime=CFDictionaryGetValue(keypressDictionary, CFSTR("time"));
			keypressModifierFlags=CFDictionaryGetValue(keypressDictionary, CFSTR("modifierFlags"));
			keypressTickCount=CFDictionaryGetValue(keypressDictionary, CFSTR("tickCount"));
			CFNumberGetValue(keypressTickCount, kCFNumberDoubleType, &keypressTickCountDouble);
			keypressAddress=CFDictionaryGetValue(keypressDictionary, CFSTR("keyCode"));
			CFNumberGetValue(keypressAddress, kCFNumberDoubleType, &keypressAddressDouble);
			
			characterRange.location=0;
			characterRange.length=1;
			CFStringGetCharacters(keypressCharacter, characterRange, keypressCharacterUnicode);
			if(keypressCharacterUnicode[0] <= (UniChar)127){  //it's a UTF8, MATLAB knows how do display this
				keypressCharacterUTF8[0]=(char)(keypressCharacterUnicode[0]); //throw out what MATLAB will not print.
				keypressCharacterUTF8[1]='\0';
				//mexPrintf("character:  %s\n", keypressCharacterUTF8);
				PsychCopyOutCharArg(1, kPsychArgOptional, keypressCharacterUTF8);
			}else{
				keypressCharacterAsValue=(double)(keypressCharacterUnicode[0]);
				//mexPrintf("character:  %f\n",  keypressCharacterAsValue);
				PsychCopyOutDoubleArg(1, kPsychArgOptional, keypressCharacterAsValue);
			}
	//		CFStringGetCString(keypressCharacter, keypressCharacterUTF8, 2, kCFStringEncodingUTF8);
			CFNumberGetValue(keypressTime, kCFNumberDoubleType ,&keypressTimeDouble);
	//		mexPrintf("time:       %d\n", keypressTimeDouble);
	//		PsychCopyOutDoubleArg(2, kPsychArgOptional, keypressTimeDouble);
			

			numOutputArgs= PsychGetNumOutputArgs();
			if(numOutputArgs==2){
				alphaLock= CFDictionaryGetValue(keypressModifierFlags, CFSTR("NSAlphaShiftKeyMask"));
				CFNumberGetValue(alphaLock, kCFNumberCharType, &alphaLockCFlag);
				commandKey= CFDictionaryGetValue(keypressModifierFlags, CFSTR("NSCommandKeyMask"));
				CFNumberGetValue(commandKey, kCFNumberCharType, &commandKeyCFlag);
				controlKey= CFDictionaryGetValue(keypressModifierFlags, CFSTR("NSControlKeyMask"));
				CFNumberGetValue(controlKey, kCFNumberCharType, &controlKeyCFlag);
				optionKey= CFDictionaryGetValue(keypressModifierFlags, CFSTR("NSAlternateKeyMask"));
				CFNumberGetValue(optionKey, kCFNumberCharType, &optionKeyCFlag);
				shiftKey= CFDictionaryGetValue(keypressModifierFlags, CFSTR("NSShiftKeyMask"));
				CFNumberGetValue(shiftKey, kCFNumberCharType, &shiftKeyCFlag);
				numericKeypad= CFDictionaryGetValue(keypressModifierFlags, CFSTR("NSNumericPadKeyMask"));
				CFNumberGetValue(numericKeypad, kCFNumberCharType, &numericKeypadCFlag);
	//			helpKey= CFDictionaryGetValue(keypressModifierFlags, CFSTR("NSHelpKeyMask"));
	//			CFNumberGetValue(helpKey, kCFNumberCharType, &helpKeyCFlag);
				functionKey= CFDictionaryGetValue(keypressModifierFlags, CFSTR("NSFunctionKeyMask"));
				CFNumberGetValue(functionKey, kCFNumberCharType, &functionKeyCFlag);
				
				PsychAllocOutStructArray(2, TRUE, numStructElements, numStructFieldNames, charTimeFieldNames,  &charTimeStruct);
	//			missing from OS X
	//			PsychSetStructArrayBooleanElement("mouseButton", 0, , charTimeStruct);
	//			same in OS X and OS 9
				PsychSetStructArrayDoubleElement("ticks", 0, keypressTickCountDouble, charTimeStruct);
				PsychSetStructArrayDoubleElement("secs", 0, keypressTimeDouble, charTimeStruct);	  
				PsychSetStructArrayDoubleElement("address", 0, keypressAddressDouble, charTimeStruct);	  
				PsychSetStructArrayBooleanElement("alphaLock", 0, (psych_bool)alphaLockCFlag, charTimeStruct);
				PsychSetStructArrayBooleanElement("commandKey", 0, (psych_bool)commandKeyCFlag, charTimeStruct);
				PsychSetStructArrayBooleanElement("controlKey", 0, (psych_bool)controlKeyCFlag, charTimeStruct);
				PsychSetStructArrayBooleanElement("optionKey", 0, (psych_bool)optionKeyCFlag, charTimeStruct);
	//			new for OS X
				PsychSetStructArrayBooleanElement("shiftKey", 0, (psych_bool)shiftKeyCFlag, charTimeStruct);
				PsychSetStructArrayBooleanElement("numericKeypad", 0, (psych_bool)numericKeypadCFlag, charTimeStruct);
	//			PsychSetStructArrayBooleanElement("helpKey", 0, (psych_bool)helpKeyCFlag, charTimeStruct);
				PsychSetStructArrayBooleanElement("functionKey", 0, (psych_bool)functionKeyCFlag, charTimeStruct);
				
			}
			CFRelease(keypressDictionary);
		} //close:  if(keypressDictionary != NULL){
	}else{//  if(!lostKeyWindowFlag){
		//If we get to here, that means that GetChar was called, set its key gathering window to be the key window, and while waiting
		//for key input another window was made the key window.  How should we handle this case?  Here we return nan for both arguments.
		//If we prefer to return an error, this could be done in the .m file wrapper, "Getchar" instead of here in "CocoaEventBridge('Getchar');
		PsychCopyOutDoubleArg(1, kPsychArgOptional, PsychGetNanValue());
		PsychCopyOutDoubleArg(2, kPsychArgOptional, PsychGetNanValue());
	}
	
    return(PsychError_none);	
}






	
