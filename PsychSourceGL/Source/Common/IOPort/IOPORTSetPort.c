/*
	IOPORTSetPort.c		
  
	PROJECTS: 
	
		IOPort only.
  
	AUTHORS:
	
		Allen.Ingling@nyu.edu				awi
		mario.kleiner at tuebingen.mpg.de	mk
  
	PLATFORMS:	
	
		All.
    
	HISTORY:

		12/20/2004	awi		Wrote it.
		04/10/2008	mk		Started to extend/rewrite it to become a full-fledged generic I/O driver (serial port, parallel port, etc...).
 
	DESCRIPTION:
	
		Originally controlled the Code Mercenaries IO Warrior 40 device.
	
		It will hopefully become a generic I/O driver...

*/


#include "IOPort.h"
#if PSYCH_SYSTEM == PSYCH_OSX

#include <CoreFoundation/CoreFoundation.h>
//#include "IOWarriorLib.h"

//IOPort('SetPort', logicalBits);
//                  1

PsychError IOPORTSetPort(void)
{

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));
    
    int			i,result, m, n, p;
    UInt32		bitValue, buffer;
	double		*inputArray;
	
	
	//get the input matrix and load bits it into the UInt32
	PsychAllocInDoubleMatArg(1, kPsychArgRequired, &m, &n, &p, &inputArray);
	buffer=0;
	bitValue=1;
	for(i=0;i<32;i++){
		if(inputArray[i]!=0)
			buffer=buffer | bitValue;
		bitValue=bitValue<<1;
	}

    // writing 32 bits
//    result = IOWarriorWriteInterface0 (&buffer);
    if (result)
        printf ("IOWarriorWriteInterface0 returned %d\n", result);
	
    return(PsychError_none);	
}
#endif
