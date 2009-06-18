/*
	PsychSourceGL/Source/Common/CocoaEventBridge/CocoaEventBridgeSynopsis.c.c
  
	PROJECTS: 
  
		CocoaEventBridge only
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
  
		Only OS X.
    
	HISTORY:

		9/15/05		awi		Wrote it .

*/


#include "Screen.h"

#define MAX_SYNOPSIS_STRINGS 500  

//declare variables local to this file.  
static const char *synopsisSYNOPSIS[MAX_SYNOPSIS_STRINGS];

void InitializeSynopsis()
{
	int i=0;
	const char **synopsis = synopsisSYNOPSIS;  //abbreviate the long name

	synopsis[i++] = "Usage:";

	// Open or close a window or texture:
	synopsis[i++] = "[char, when]=CocoaEventBridge('GetChar');";
	synopsis[i++] = "avail=CocoaEventBridge('CharAvail');";
	synopsis[i++] = "CocoaEventBridge('FlushChars');";
	synopsis[i++] = "CocoaEventBridge('ListenChars');";	
	synopsis[i++] = "CocoaEventBridge('StopListenChars');";
	synopsis[i++] = "oldPath=CocoaEventBridge('SetCocoaBridgeBundlePath' [,newPath]);";

	synopsis[i++] = NULL;  //this tells PsychDisplayScreenSynopsis where to stop
	if (i > MAX_SYNOPSIS_STRINGS) {
		PrintfExit("%s: increase dimension of synopsis[] from %ld to at least %ld and recompile.",__FILE__,(long)MAX_SYNOPSIS_STRINGS,(long)i);
	}
}


PsychError DisplayEventBridgeSynopsis(void)
{
	int i;
	static psych_bool	isSynopsisInitialized=FALSE;
	
	if(!isSynopsisInitialized){
		InitializeSynopsis();
		isSynopsisInitialized=TRUE;
	}
	
	for (i = 0; synopsisSYNOPSIS[i] != NULL; i++)
		printf("%s\n",synopsisSYNOPSIS[i]);
		
	return(PsychError_none);
}





