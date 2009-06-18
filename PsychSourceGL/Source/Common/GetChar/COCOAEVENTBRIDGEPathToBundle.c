/*
	PsychSourceGL/Source/Common/CocoaEventBridge/COCOAEVENTBRIDGEPathToBundle.c		
  
	PROJECTS: 
  
		CocoaEventBridge only
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
  
		Only OS X.
    
	HISTORY:

		9/20/05		awi		Wrote it .


*/

#include "EventBridgeBundleHeader.h"
//#include "CocoaEventBridgeHelpers.h"
#include "CocoaEventBridge.h"

//file static variables to retain the path name
static	char		*bundleFilePathNameStr=NULL;



static char useString[] = "oldPath=CocoaEventBridge(['PathToBundle']);";
static char synopsisString[] = 
        "Get and optionally set the path to the bundle \"GetCharWindow.bundle\"."
		"\n\n"
		"Do not call CocoaEventBridge('PathToBundle') directly.  It should be used by only the Psychtoolbox function \"InitCocoaEventBridge\"";
static char seeAlsoString[] = "GetChar";	

PsychError COCOAEVENTBRIDGEPathToBundle(void) 
{

	psych_bool						isNewArgThereFlag;
	int							newNameLength;
	char						*tempBundleFilePathNameStr, *localBundleFilePathNameStr;
	

	//all subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));
	
	//first copy out the old path 
	GetPathToBundleString(&localBundleFilePathNameStr);
	PsychCopyOutCharArg(1, kPsychArgOptional, localBundleFilePathNameStr);
	
	//read in and retain the new name.  It arrives in a temporary variable
	isNewArgThereFlag=PsychAllocInCharArg(1, kPsychArgOptional, &tempBundleFilePathNameStr);
	if(isNewArgThereFlag){
		newNameLength=strlen(tempBundleFilePathNameStr);
		if(bundleFilePathNameStr != NULL)
			free((void*)bundleFilePathNameStr);
		bundleFilePathNameStr=(char*)malloc(sizeof(char) * (newNameLength + 1));
		strncpy(bundleFilePathNameStr, tempBundleFilePathNameStr, newNameLength + 1);
	}
		
    return(PsychError_none);	
}


void GetPathToBundleString(char **fPath)
{
	if(bundleFilePathNameStr==NULL){
			bundleFilePathNameStr=(char *)malloc(sizeof(char));
			bundleFilePathNameStr[0]='\0';
	}
	*fPath=bundleFilePathNameStr;

}


void FreePathToBundleString(void)
{
	if(bundleFilePathNameStr != NULL){
		free((void*)bundleFilePathNameStr);
		bundleFilePathNameStr=NULL;
	}
}






	
