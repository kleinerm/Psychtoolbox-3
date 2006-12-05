/*
	PsychToolbox/Source/Common/PsychHID/PsychHIDErrors.c		
	
	PROJECTS: 
	
		PsychHID
	
	PLATFORMS:
	
		OSX  
	
	AUTHORS:
	
		denis.pelli@nyu.edu  dgp
	  
	HISTORY:
	
		4/24/05  dgp		Wrote it.
		10/11/05 awi		Cosmetic
	
	TO DO:

		 PsychHIDErrors is used as a subroutine by PsychHIDGetReport.c and PsychHIDSetReport.c to 
		 look up the name and description strings associated with an Apple error code. At present
		 the list of errors is quite short and useful only to those two routines. However, we
		 could extend this scheme arbitrarily to include a large number of errors. I suppose
		 that the switch statement is fast, so the only cost to making this list huge would be
		 the storage space required for all the strings. However, if we suppose 128 bytes per
		 error, even 10,000 errors would demand only 1 MB of memory.
		 
		 PsychHIDGetReport.c and PsychHIDSetReport.c have essentially the same code to create an
		 err struct with three fields: n (the error number, e.g. 0xe0004061), name 
		 (e.g. 'kIOUSBUnknownPipeErr'), and description (e.g. see below). I now think it would have 
		 been cleaner to put that code here, so you would call this routine to create the "err" struct. 
		 For full generality it should also allow creation of the err struct with whatever name and 
		 description are provided by the caller.
 
 */
#include "PsychHID.h"
//#include <USB.h>

int PsychHIDErrors(int error,char **namePtr,char **descriptionPtr);
int PsychHIDErrors(int error,char **namePtr,char **descriptionPtr)
{
	assert(namePtr!=NULL && descriptionPtr!=NULL);
	switch(error){
	// IOUSBFamily error codes. USB.h Revision 1.44  2003/09/10 19:07:17 nano
	// path /System/Library/Frameworks/Kernel.framework/Versions/A/Headers/IOKit/usb/USB.h
	case 0xe000404b: *namePtr="kIOUSBHighSpeedSplitError"; *descriptionPtr="Error to hub on high speed bus trying to do split transaction."; break;
	case 0xe000404c: *namePtr="kIOUSBLowLatencyFrameListNotPreviouslyAllocated"; *descriptionPtr="Attempted to use user land low latency isoc calls w/out calling PrepareBuffer (on the frame list) first."; break;
	case 0xe000404d: *namePtr="kIOUSBLowLatencyBufferNotPreviouslyAllocated"; *descriptionPtr="Attempted to use user land low latency isoc calls w/out calling PrepareBuffer (on the data buffer) first ."; break;
	case 0xe000404e: *namePtr="kIOUSBInterfaceNotFound"; *descriptionPtr="Interface ref not recognized."; break;
	case 0xe000404f: *namePtr="kIOUSBPipeStalled"; *descriptionPtr="Pipe has stalled, error needs to be cleared."; break;
	case 0xe0004050: *namePtr="kIOUSBTransactionReturned"; *descriptionPtr="The transaction has been returned to the caller."; break;
	case 0xe0004051: *namePtr="kIOUSBTransactionTimeout"; *descriptionPtr="Transaction timed out."; break;
	case 0xe0004056: *namePtr="kIOUSBConfigNotFound"; *descriptionPtr="Configuration not found."; break;
	case 0xe0004057: *namePtr="kIOUSBEndpointNotFound"; *descriptionPtr="Endpoint not found."; break;
	case 0xe000405d: *namePtr="kIOUSBNotEnoughPowerErr"; *descriptionPtr="Not enough power for selected configuration."; break;
	case 0xe000405e: *namePtr="kIOUSBNotEnoughPipesErr"; *descriptionPtr="Not enough pipes in interface."; break;
	case 0xe000405f: *namePtr="kIOUSBNoAsyncPortErr"; *descriptionPtr="No async port."; break;
	case 0xe0004060: *namePtr="kIOUSBTooManyPipesErr"; *descriptionPtr="Too many pipes."; break;
	case 0xe0004061: *namePtr="kIOUSBUnknownPipeErr"; *descriptionPtr="Pipe ref not recognized."; break;
	// IOKit error codes. IOReturn.h. Source: "Black & Bleu" application. http://www.bleurose.com/pages/blableu.html
	// path /System/Library/Frameworks/IOKit.framework/Versions/A/Headers/IOReturn.h
	case 0xe00002bc: *namePtr="kIOReturnError"; *descriptionPtr="A general error was detected by one of the general routines of the I/O kit framework."; break;
	case 0xe00002c7: *namePtr="kIOReturnUnsupported"; *descriptionPtr="An unsupported function was called by one of the general routines of the I/O Kit framework."; break;
	case 0xe00002d9: *namePtr="kIOReturnNotAttached"; *descriptionPtr="A 'device not attached' condition was detected by one of the general routines of the I/O Kit framework."; break;
	case 0xe00002ed: *namePtr="kIOReturnNotResponding"; *descriptionPtr="A 'device not responding' condition was detected by one of the general routines of the I/O Kit framework."; break;
	case 0: *namePtr=""; *descriptionPtr=""; break;
	default: return 1;
	}
	return 0;
}

