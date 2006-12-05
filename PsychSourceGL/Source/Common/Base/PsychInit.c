/*
  PsychToolbox2/Source/Common/PsychInit.cpp		
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS: All
  
  PROJECTS:
  08/25/02	awi		Screen on MacOS9
   

  HISTORY:
  08/25/02  awi		wrote it.  
  
  DESCRIPTION:
  
	Holds the master initialization for the Psychtoolbox function.
	Sub-section hold their own inits which are called by the master
	init function contained herein.  After the Psychtoolbox library
	has initialzed itself in then invokes the project init which 
	must be named PsychModuleInit(). 
        
        Each PsychToolbox module should register its subfuctions within 
        its PsychModuleInit(). 
   

*/

#include "Psych.h"


PsychError PsychInit(void)
{
	//On windows this sets our pointers to functions
	// within Matlab.exe.  On every other platform
	//those are set at compile time using libraries.  
//	PsychLinkMatlab();

	//first init Psychtoolbox libraries
	InitPsychError();
	InitPsychAuthorList();
	PsychInitTimeGlue();
	
	//Registration of the Psychtoolbox exit function is 
	//done in ScriptingGlue.cpp because how that is done is 
	//specific to the scripting environment. Note that registration
	//of the project exit function is done within the project init.  
	
	//then call call the project init.  
	PsychModuleInit();
	
	return(PsychError_none);

}


/* PsychExit is the function invoked last before the module is 
   purged.  It is abstracted to be unspecific to the scripting
   language.  The language-specific versions are named PsychExitGlue()
   and they are found in SciptingGlue.cpp  
 */
PsychError PsychExit(void)
{	
	PsychFunctionPtr projectExit;
	PsychError error;

	projectExit = PsychGetProjectExitFunction();
	if(projectExit !=NULL)
		//PsychErrorExitMsg((PsychError)(*projectExit)(),NULL);
		error=(*projectExit)();

	//put whatever cleanup of the Psychtoolbox is required here.
	
	return(PsychError_none);
}


