/*
	GetEthernetAddress.c		

	PROJECTS:
	
		Screen

	PLATFORMS:  
	
		Only OS X

	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 
		
	DESCRIPTION:
	
		Returns the ethernet address of the primary ethernet port.  This is a unique identifier associated with the ethernet hardware.
		
	    Code lifted from Apple's GetPrimaryMACAddress.c example here:
		http://developer.apple.com/samplecode/GetPrimaryMACAddress/listing1.html		
		
	HISTORY:
		
		12/8/04		awi		Created file. Borrowed FindEthernetInterfaces() and GetMACAddress().  Wrote some stuff to turn it into a string.    
		
  
  
*/

#include "Psych.h"
#include <CoreFoundation/CoreFoundation.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/network/IOEthernetInterface.h>
#include <IOKit/network/IONetworkInterface.h>
#include <IOKit/network/IOEthernetController.h>

//from Apple
static kern_return_t FindEthernetInterfaces(io_iterator_t *matchingServices);
static kern_return_t GetMACAddress(io_iterator_t intfIterator, UInt8 *MACAddress);
//for Psychtoolbox
static psych_bool GetPrimaryEthernetAddress(UInt8 *MACAddress);
static int GetEthernetAddressLengthBytes(void);


// Returns an iterator containing the primary (built-in) Ethernet interface. The caller is responsible for
// releasing the iterator after the caller is done with it.
static kern_return_t FindEthernetInterfaces(io_iterator_t *matchingServices)
{
    kern_return_t    kernResult; 
    mach_port_t      masterPort;
    CFMutableDictionaryRef  matchingDict;
    CFMutableDictionaryRef  propertyMatchDict;
    
    // Retrieve the Mach port used to initiate communication with I/O Kit
    kernResult = IOMasterPort(MACH_PORT_NULL, &masterPort);
    if (KERN_SUCCESS != kernResult)
    {
        printf("IOMasterPort returned %d\n", kernResult);
        return kernResult;
    }
    
    // Ethernet interfaces are instances of class kIOEthernetInterfaceClass. 
    // IOServiceMatching is a convenience function to create a dictionary with the key kIOProviderClassKey and 
    // the specified value.
    matchingDict = IOServiceMatching(kIOEthernetInterfaceClass);

    // Note that another option here would be:
    // matchingDict = IOBSDMatching("en0");
        
    if (NULL == matchingDict)
    {
        printf("IOServiceMatching returned a NULL dictionary.\n");
    }
    else {
        // Each IONetworkInterface object has a psych_bool property with the key kIOPrimaryInterface. Only the
        // primary (built-in) interface has this property set to TRUE.
        
        // IOServiceGetMatchingServices uses the default matching criteria defined by IOService. This considers
        // only the following properties plus any family-specific matching in this order of precedence 
        // (see IOService::passiveMatch):
        //
        // kIOProviderClassKey (IOServiceMatching)
        // kIONameMatchKey (IOServiceNameMatching)
        // kIOPropertyMatchKey
        // kIOPathMatchKey
        // kIOMatchedServiceCountKey
        // family-specific matching
        // kIOBSDNameKey (IOBSDNameMatching)
        // kIOLocationMatchKey
        
        // The IONetworkingFamily does not define any family-specific matching. This means that in            
        // order to have IOServiceGetMatchingServices consider the kIOPrimaryInterface property, we must
        // add that property to a separate dictionary and then add that to our matching dictionary
        // specifying kIOPropertyMatchKey.
            
        propertyMatchDict = CFDictionaryCreateMutable( kCFAllocatorDefault, 0,
                                                       &kCFTypeDictionaryKeyCallBacks,
                                                       &kCFTypeDictionaryValueCallBacks);
    
        if (NULL == propertyMatchDict)
        {
            printf("CFDictionaryCreateMutable returned a NULL dictionary.\n");
        }
        else {
            // Set the value in the dictionary of the property with the given key, or add the key 
            // to the dictionary if it doesn't exist. This call retains the value object passed in.
            CFDictionarySetValue(propertyMatchDict, CFSTR(kIOPrimaryInterface), kCFBooleanTrue); 
            
            // Now add the dictionary containing the matching value for kIOPrimaryInterface to our main
            // matching dictionary. This call will retain propertyMatchDict, so we can release our reference 
            // on propertyMatchDict after adding it to matchingDict.
            CFDictionarySetValue(matchingDict, CFSTR(kIOPropertyMatchKey), propertyMatchDict);
            CFRelease(propertyMatchDict);
        }
    }
    
    // IOServiceGetMatchingServices retains the returned iterator, so release the iterator when we're done with it.
    // IOServiceGetMatchingServices also consumes a reference on the matching dictionary so we don't need to release
    // the dictionary explicitly.
    kernResult = IOServiceGetMatchingServices(masterPort, matchingDict, matchingServices);    
    if (KERN_SUCCESS != kernResult)
    {
        printf("IOServiceGetMatchingServices returned %d\n", kernResult);
    }
        
    return kernResult;
}
    
// Given an iterator across a set of Ethernet interfaces, return the MAC address of the last one.
// If no interfaces are found the MAC address is set to an empty string.
// In this sample the iterator should contain just the primary interface.
static kern_return_t GetMACAddress(io_iterator_t intfIterator, UInt8 *MACAddress)
{
    io_object_t    intfService;
    io_object_t    controllerService;
    kern_return_t  kernResult = KERN_FAILURE;
    
    // Initialize the returned address
    bzero(MACAddress, kIOEthernetAddressSize);
    
    // IOIteratorNext retains the returned object, so release it when we're done with it.
    while ((intfService = IOIteratorNext(intfIterator)))
    {
        CFTypeRef  MACAddressAsCFData;        

        // IONetworkControllers can't be found directly by the IOServiceGetMatchingServices call, 
        // since they are hardware nubs and do not participate in driver matching. In other words,
        // registerService() is never called on them. So we've found the IONetworkInterface and will 
        // get its parent controller by asking for it specifically.
        
        // IORegistryEntryGetParentEntry retains the returned object, so release it when we're done with it.
        kernResult = IORegistryEntryGetParentEntry( intfService,
                                                    kIOServicePlane,
                                                    &controllerService );

        if (KERN_SUCCESS != kernResult)
        {
            printf("IORegistryEntryGetParentEntry returned 0x%08x\n", kernResult);
        }
        else {
            // Retrieve the MAC address property from the I/O Registry in the form of a CFData
            MACAddressAsCFData = IORegistryEntryCreateCFProperty( controllerService,
                                                                  CFSTR(kIOMACAddress),
                                                                  kCFAllocatorDefault,
                                                                  0);
            if (MACAddressAsCFData)
            {
                //CFShow(MACAddressAsCFData); // for display purposes only; output goes to stderr
                
                // Get the raw bytes of the MAC address from the CFData
                CFDataGetBytes(MACAddressAsCFData, CFRangeMake(0, kIOEthernetAddressSize), MACAddress);
                CFRelease(MACAddressAsCFData);
            }
                
            // Done with the parent Ethernet controller object so we release it.
            (void) IOObjectRelease(controllerService);
        }
        
        // Done with the Ethernet interface object so we release it.
        (void) IOObjectRelease(intfService);
    }
        
    return kernResult;
}


//Stuff Allen Added for PTB.


/*
	GetEthernetAddressLengthBytes()
	
	GetPrimaryDeviceEthernetAddress() returns fills an array of UInt8 values which the caller allocates.  The caller should call GetEthernetAddressLengthBytes()
	first to find the length of the array which it must allocate.  
	
*/

static int GetEthernetAddressLengthBytes(void)
{
	//cross-platform abstraction in case we expose this.   
	return(kIOEthernetAddressSize);
}


static psych_bool GetPrimaryEthernetAddress(UInt8 *MACAddress)
{
	psych_bool			errorReturn;
    kern_return_t	kernResult; 
    io_iterator_t	intfIterator;
 
	errorReturn=FALSE;
    kernResult = FindEthernetInterfaces(&intfIterator);
	if(kernResult != KERN_SUCCESS)
		errorReturn=TRUE;
    else{
        kernResult = GetMACAddress(intfIterator, MACAddress);
        if(kernResult != KERN_SUCCESS)
			errorReturn=TRUE;
    }
    
    (void) IOObjectRelease(intfIterator);  // Release the iterator.
        
    return(errorReturn);	
}


/*
	Fill address str with string naming the MAC address of the primary ethernet interface.  If there is problem
	getting that address then return the empty string.
	
	The caller should allocate memory to hold the result.  To find out how much memory to allocate, call 
	GetPrimaryEthernetAddressStringLengthBytes() and add 1, for the string terminator,  to the result.
	In practice it should always be fine to use a static string such as Str255 without risk of overflow.     
*/
int GetPrimaryEthernetAddressString(char *addressStr, psych_bool capsFlag, psych_bool colonSeparatedFlag)
{
	int			i, tempDigitValue, addressStringLength, addressLengthBytes, numCharsPrinted, currentPosition;
	UInt8		MACAddress[256];
	psych_bool		errorResult;
	Str255		tempRawStrBuffer, tempDigitStrBuffer, tempResultBuffer;
	
	addressLengthBytes= GetEthernetAddressLengthBytes();
	errorResult=GetPrimaryEthernetAddress(MACAddress);
	if(errorResult)
		tempResultBuffer[0]= '\0';
	else{
		//iterate over the bytes in the address and convert them into two-character hex representations. Concate them into the string.  
		for(i=0;i<addressLengthBytes;i++){
			tempDigitValue=(int)MACAddress[i];
			if(capsFlag)
				numCharsPrinted=sprintf((char*) tempDigitStrBuffer, "%02X", tempDigitValue);
			else
				numCharsPrinted=sprintf((char*) tempDigitStrBuffer, "%02x", tempDigitValue);
			tempRawStrBuffer[i*2]=tempDigitStrBuffer[0];
			tempRawStrBuffer[i*2+1]=tempDigitStrBuffer[1];
		}
		tempRawStrBuffer[addressLengthBytes*2]='\0';
		
		//insert colons in the ethernet address if requested
		if(!colonSeparatedFlag)
			strcpy((char*) tempResultBuffer, (char*) tempRawStrBuffer);
		else{
			currentPosition=0;
			for(i=0;i<addressLengthBytes;i++){
				tempResultBuffer[currentPosition]=tempRawStrBuffer[i*2];
				++currentPosition;
				tempResultBuffer[currentPosition]=tempRawStrBuffer[i*2+1];
				++currentPosition;
				tempResultBuffer[currentPosition]=':';
				++currentPosition;
			}
			tempResultBuffer[currentPosition-1]='\0';
		}
	}
	addressStringLength=strlen((char*) tempResultBuffer);
	if(addressStr!=NULL)
		strcpy(addressStr, (char*) tempResultBuffer);
	return(addressStringLength);

}

int GetPrimaryEthernetAddressStringLengthBytes(psych_bool colonSeparatedFlag)
{
	return(GetPrimaryEthernetAddressString(NULL, TRUE, colonSeparatedFlag));
}
