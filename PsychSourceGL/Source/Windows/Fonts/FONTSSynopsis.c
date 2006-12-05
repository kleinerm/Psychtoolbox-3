/*
  PsychToolbox2/Source/OSX/Fonts/FONTSSynopsis.cpp	
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS: All OS
  
  PROJECTS:
  08/21/02	awi		OS X version
   

  HISTORY:
  08/21/02  awi		Wrote it.  
  
  DESCRIPTION:
  

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


        synopsis[i++] = "numFonts=Fonts('NumFonts')";
	synopsis[i++] = "fontInfoStructArray=Fonts('Fonts')";
        synopsis[i++] = "versionInfo=Fonts('Version')";


	synopsis[i++] = NULL;  //this tells PsychDisplayScreenSynopsis where to stop
	if (i > MAX_SYNOPSIS_STRINGS) {
		PrintfExit("%s: increase dimension of synopsis[] from %ld to at least %ld and recompile.",__FILE__,(long)MAX_SYNOPSIS_STRINGS,(long)i);
	}
}


PsychError PsychDisplayATSSynopsis(void)
{
	int i;
	
	for (i = 0; synopsisSYNOPSIS[i] != NULL; i++)
		printf("%s\n",synopsisSYNOPSIS[i]);
		
	return(PsychError_none);
}





