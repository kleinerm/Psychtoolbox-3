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

//int GetPrimaryEthernetAddressStringLengthBytes(psych_bool colonSeparatedFlag);
int GetPrimaryEthernetAddressString(char *addressStr, psych_bool capsFlag, psych_bool colonSeparatedFlag);
int GetPrimaryEthernetAddressStringLengthBytes(psych_bool colonSeparatedFlag);


//end include once
#endif


