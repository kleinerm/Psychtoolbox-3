/*
	PsychToolbox2/Source/Common/PsychAuthors.h		
    
	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	Only OSX for now.
    
	DESCRIPTION:
		
		Part of the version information returned by a Psychtoolbox mex file includes its authors.  The master author list is contained in 
		PsychAuthors.c. If contribute to a Psychtoolbox mex file, then please add yourself to this file.  Individual authors are enabled
		for specific mex files by calling PsychSetModuleAuthorByInitials().  Authorship indicates authorship of specific mex files, not of 
		the foundation Psychtoolbox functions upon which they depend.  So, do not add me (Allen Ingling) if you create a brand new mex file,
		even if your mex functions call other Psychtoolbox functions which I have written.  
		
	HISTORY:

		7/22/04		awi		Created. 
		7/27/04		awi		Added description
 
  
*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_PsychAuthors
#define PSYCH_IS_INCLUDED_PsychAuthors

#include "Psych.h"



typedef struct
{
	psych_bool		enabled;
	char		firstName[32];
	char		middleName[32];
	char		lastName[32];
	char		initials[8];
	char		email[512];
	char		url[512];
} PsychAuthorDescriptorType;

//for use by Psychtoolbox inits.  Don't call this from modules.
void InitPsychAuthorList(void);

//exports for modules (a.k.a mex targets)
void PsychSetModuleAuthorByInitials(char *initials);
int PsychGetNumModuleAuthors(void);
void GetModuleAuthorDescriptorFromIndex(int index, PsychAuthorDescriptorType **descriptor);


//end include once
#endif
	
