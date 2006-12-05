/*
	GetEthernetAddress.h		

	PROJECTS:
	
		Screen

	PLATFORMS:  
	
		Only OS X

	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 
		
	DESCRIPTION:
	
		Returns the ethernet address of the primary ethernet port as a string.  
				
		
	HISTORY:
		
		12/8/04		awi		Created file.   
		
  
  
*/


//begin include once 
#ifndef PSYCH_IS_INCLUDED_GetEthernetAddress
#define PSYCH_IS_INCLUDED_GetEthernetAddress

//int GetPrimaryEthernetAddressStringLengthBytes(Boolean colonSeparatedFlag);
int GetPrimaryEthernetAddressString(char *addressStr, Boolean capsFlag, Boolean colonSeparatedFlag);
int GetPrimaryEthernetAddressStringLengthBytes(Boolean colonSeparatedFlag);


//end include once
#endif


