/*
  PsychToolbox2/Source/Common/PsychHID/PsychHIDSynopsis.c	
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS: OSX
  
  PROJECTS: PsychHID
   

  HISTORY:
  06/20/03  awi		Wrote it.  Based on ScreenSynopsis  
  
*/
#include "Screen.h"
void InitializeSynopsis(void); // I added this prototype to make the compiler happy. dgp.

#define MAX_SYNOPSIS_STRINGS 500  

//declare variables local to this file.  
static const char *synopsisSYNOPSIS[MAX_SYNOPSIS_STRINGS];

void InitializeSynopsis(void)
{
	int i=0;
	const char **synopsis = synopsisSYNOPSIS;  // abbreviate the long name

	synopsis[i++] = "Usage:";

	synopsis[i++] = "numberOfDevices=PsychHID('NumDevices')";
	synopsis[i++] = "numberOfElements=PsychHID('NumElements',deviceNumber)";
	synopsis[i++] = "numberOfCollections=PsychHID('NumCollections',deviceNumber)";
    synopsis[i++] = "devices=PsychHID('Devices')";
	synopsis[i++] = "elements=PsychHID('Elements',deviceNumber)";
	synopsis[i++] = "collections=PsychHID('Collections',deviceNumber)";
	synopsis[i++] = "elementState=PsychHID('RawState',deviceNumber,elementNumber)";
	synopsis[i++] = "elementState=PsychHID('CalibratedState',deviceNumber,elementNumber)";
	synopsis[i++] = "[keyIsDown,secs,keyCode]=PsychHID('KbCheck',[deviceNumber])";
	synopsis[i++] = "[report,err]=PsychHID('GetReport',deviceNumber,reportType,reportID,reportBytes)";
	synopsis[i++] = "err=PsychHID('SetReport',deviceNumber,reportType,reportID,report)";

	synopsis[i++] = NULL;  // this tells PsychDisplayPsychHIDSynopsis where to stop
	if (i > MAX_SYNOPSIS_STRINGS) {
		PrintfExit("%s: increase dimension of synopsis[] from %ld to at least %ld and recompile.",__FILE__,(long)MAX_SYNOPSIS_STRINGS,(long)i);
	}
}


PsychError PsychDisplayPsychHIDSynopsis(void)
{
	int i;
	
	for (i = 0; synopsisSYNOPSIS[i] != NULL; i++)
		printf("%s\n",synopsisSYNOPSIS[i]);
		
	return(PsychError_none);
}





