/*
  PsychToolbox2/Source/Common/PsychAuthors.c		
    
  AUTHORS:

	Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	All.
    

  HISTORY:
  7/22/04  awi		Created. 
 
  
*/


#include "Psych.h"
// I added this prototype to make the compiler happy. dgp.
void PsychAddAuthor(char *firstName,char *middleName,char *lastName,char *initials,char *email,char *url);

#define	MAX_PSYCHTOOLBOX_AUTHORS	25

static int							numAuthors=0;
static PsychAuthorDescriptorType	authorList[MAX_PSYCHTOOLBOX_AUTHORS];

void PsychAddAuthor(
		char *firstName,
		char *middleName,
		char *lastName,
		char *initials,
		char *email,
		char *url
)
{
	if(strlen(firstName)>31)
		PsychErrorExitMsg(PsychError_stringOverrun, "Name string too long");
	if(strlen(middleName)>31)
		PsychErrorExitMsg(PsychError_stringOverrun, "Name string too long");
	if(strlen(lastName)>31)
		PsychErrorExitMsg(PsychError_stringOverrun, "Name string too long");
	if(strlen(initials)>7)
		PsychErrorExitMsg(PsychError_stringOverrun, "Initial string too long");
	if(strlen(email)>511)
		PsychErrorExitMsg(PsychError_stringOverrun, "Email string too long");
	if(strlen(url)>511)
		PsychErrorExitMsg(PsychError_stringOverrun, "URL string too long");
	
	strcpy(authorList[numAuthors].firstName, firstName);
	strcpy(authorList[numAuthors].middleName, middleName);
	strcpy(authorList[numAuthors].lastName, lastName);
	strcpy(authorList[numAuthors].initials, initials);
	strcpy(authorList[numAuthors].email, email);
	strcpy(authorList[numAuthors].url, url);
	
	authorList[numAuthors].enabled=FALSE;
	++numAuthors;
}


void PsychSetModuleAuthorByInitials(char *initials)
{
	int			i;
	int			numFound;
	
	numFound=0;
	for(i=0;i<numAuthors;i++){
		if(!strcmp(initials, authorList[i].initials)){
			if(numFound==1){
				PsychErrorExitMsg(PsychError_internal, "Attempt to set module author using ambiguous initials.");
			}else{
				authorList[i].enabled=TRUE;
				++numAuthors;
			}
		}	
	}
}

void InitPsychAuthorList(void)
{
	PsychAddAuthor(
		"Allen",
		"W.",
		"Ingling",
		"awi",
		"Allen.Ingling@nyu.edu",
		""
	);
	PsychAddAuthor(
		"Denis",
		"G.",
		"Pelli",
		"dgp",
		"Denis.Pelli@nyu.edu",
		"http://www.psych.nyu.edu/pelli/"
	);
	PsychAddAuthor(
		"David",
		"H.",
		"Brainard",
		"dhb",
		"brainard@cattell.psych.upenn.edu",
		"http://color.psych.upenn.edu/brainard/"
	);
	PsychAddAuthor(
	//  Contributed Screen 'DrawDots'
		"Keith",
		"A.",
		"Schneider",
		"kas",
		"kas@princeton.edu",
		""
	);
	PsychAddAuthor(
	// Contributed Screen 'DrawDots'
		"Finnegan",
		"J.",
		"Calabro",
		"fjc",
		"fcalabro@bu.edu",
		""
	);
	PsychAddAuthor(
	// GL bug fixes 
		"Mario",
		"",
		"Kleiner",
		"mk",
		"mario.kleiner@tuebingen.mpg.de",
		"http://www.kyb.mpg.de/~kleinerm"
	);
	PsychAddAuthor(
	// Port to OS-X on Intel-Macs, and misc stuff: 
		"Christopher",
		"",
		"Broussard",
		"cb",
		"chrg@sas.upenn.edu",
		"http://color.psych.upenn.edu/lab/"
	);
}


int PsychGetNumModuleAuthors(void)
{
	int		i,numModuleAuthors;
	
	numModuleAuthors=0;
	for(i=0;i<numAuthors;i++){
		if(authorList[i].enabled)
			++numModuleAuthors;
	}
	return(numModuleAuthors);
}

/*
	GetModuleAuthorDescriptorFromIndex()
	
	The list is zero-indexed.
*/
void GetModuleAuthorDescriptorFromIndex(int index, PsychAuthorDescriptorType **descriptor)
{
	int		i, authorNumber;

	authorNumber=0;
	for(i=0;i<numAuthors;i++){
		if(authorList[i].enabled){
			if(index==authorNumber){
				*descriptor=&authorList[i];
				return;
			}
			++authorNumber;
		}
	}
	PsychErrorExitMsg(PsychError_internal, "Failed to find author for index");
}


