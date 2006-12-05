/*
	IOPORTSetPort.c		
  
	PROJECTS: 
	
		IOPort only.  
  
	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
	
		Only OS X for now
    
	HISTORY:

		12/20/2004	awi		Wrote it.
 
	DESCRIPTION:
	
		Controls the Code Mercenaries IO Warrior 40 device.  
	

*/


#include "IOPort.h"
#include <CoreFoundation/CoreFoundation.h>
#include "IOWarriorLib.h"

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
    result = IOWarriorWriteInterface0 (&buffer);
    if (result)
        printf ("IOWarriorWriteInterface0 returned %d\n", result);
	
    return(PsychError_none);	
}


	
