/*
 *  PsychHIDGenericUSBSupport.c
 *
 *  Platform: OS X
 *
 *  Authors:
 *  Christopher Broussard <chrg@sas.upenn.edu>		cgb
 */

#include "PsychHID.h"

// Function Declarations
IOReturn ConfigureDevice(IOUSBDeviceInterface **dev);

// Globals
extern GENERIC_USB_TYPE g_GenericUSBTracker[PSYCH_HID_MAX_GENERIC_USB_DEVICES];

bool PsychHIDControlTransfer(int usbHandle, psych_uint8 bmRequestType, psych_uint16 wValue, psych_uint16 wIndex, psych_uint16 wLength, void *pData)
{
	IOUSBDeviceInterface **dev;
	bool retVal = true;
	
	// Setup the USB request data structure.
	IOUSBDevRequest request;
	request.bmRequestType = bmRequestType;
	request.wValue = wValue;
	request.wLength = wLength;
	request.wIndex = wIndex;
	request.pData = pData;
	
	dev = g_GenericUSBTracker[usbHandle];
	if (dev == NULL) {
		PsychErrMsgTxt("(PsychHIDControlTransfer) USB handle points to NULL device.");
	}
	
	// Send the data across the USB bus.
	if ((*dev)->DeviceRequest(dev, &request) != kIOReturnSuccess) {
		retVal = false;
	}
		
	return retVal;
}


void PSYCHHIDCloseUSBDevice(int usbHandle)
{
	IOUSBDeviceInterface **dev = g_GenericUSBTracker[usbHandle];
	
	if (dev != NULL) {
		(void)(*dev)->USBDeviceClose(dev);
		(void)(*dev)->Release(dev);
		dev = NULL;
	}
}


GENERIC_USB_TYPE PSYCHHIDOpenUSBDevice(int vendorID, int deviceID)
{
	mach_port_t             masterPort;
	kern_return_t           kr;
	CFMutableDictionaryRef  matchingDict;
	SInt32                  usbVendor = (SInt32)vendorID;
	SInt32                  usbProduct = (SInt32)deviceID;
	IOUSBDeviceInterface    **dev = NULL;
	io_iterator_t           iterator;
	IOCFPlugInInterface     **plugInInterface = NULL;
	HRESULT                 result;
	io_service_t            usbDevice;
	SInt32                  score;
	UInt16                  vendor;
	UInt16                  product;
	UInt16                  release;
	bool					deviceFound = false;
	
	// Create a master port for communication with the I/O Kit
	kr = IOMasterPort(MACH_PORT_NULL, &masterPort);
	if (kr || !masterPort) {
		PsychErrMsgTxt("Couldn't create a master I/O Kit port.");
	}
	
	// Set up matching dictionary for class IOUSBDevice and its subclasses
	matchingDict = IOServiceMatching(kIOUSBDeviceClassName);
	if (!matchingDict) {
		mach_port_deallocate(mach_task_self(), masterPort);
		PsychErrMsgTxt("Couldn't create a USB matching dictionary.");
	}
	
	//Add the vendor and product IDs to the matching dictionary.
	//This is the second key in the table of device-matching keys of the
	//USB Common Class Specification
	CFDictionarySetValue(matchingDict, CFSTR(kUSBVendorName),
						 CFNumberCreate(kCFAllocatorDefault,
										kCFNumberSInt32Type, &usbVendor));
	
	CFDictionarySetValue(matchingDict, CFSTR(kUSBProductName),
						 CFNumberCreate(kCFAllocatorDefault,
										kCFNumberSInt32Type, &usbProduct));
	
	kr = IOServiceGetMatchingServices(masterPort, matchingDict, &iterator);
	if (kr) {
		PsychErrMsgTxt("Couldn't get matching services\n");
	}
	
	// Attempt to find the correct device.
	while (usbDevice = IOIteratorNext(iterator)) {
		// Create an intermediate plug-in
		kr = IOCreatePlugInInterfaceForService(usbDevice,
											   kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID,
											   &plugInInterface, &score);
		
		// Don?t need the device object after intermediate plug-in is created
		kr = IOObjectRelease(usbDevice);
		if ((kIOReturnSuccess != kr) || !plugInInterface) {
			printf("Unable to create a plug-in (%08x)\n", kr);
			continue;
		}
		
		// Now create the device interface
		result = (*plugInInterface)->QueryInterface(plugInInterface,
													CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID),
													(LPVOID *)&dev);
		
		// Don?t need the intermediate plug-in after device interface is created.
		(*plugInInterface)->Release(plugInInterface);
		
		if (result || !dev) {
			printf("Couldn't create a device interface (%08x)\n", (int) result);
			continue;
		}
		
		// Check these values for confirmation.
		kr = (*dev)->GetDeviceVendor(dev, &vendor);
		kr = (*dev)->GetDeviceProduct(dev, &product);
		kr = (*dev)->GetDeviceReleaseNumber(dev, &release);
		if ((vendor != vendorID) || (product != deviceID)) {
			printf("Found unwanted device (vendor = %d, device = %d)\n", vendor, product);
			(void) (*dev)->Release(dev);
			continue;
		}
		else {
			deviceFound = true;
			//printf("Vendor: 0x%x\nProduct: 0x%x\nRelease: 0x%x\n", vendor, product, release);
			break;
		}
		
	}
	
	if (deviceFound) {
		// Open the device to change its state
		kr = (*dev)->USBDeviceOpen(dev);
		if (kr != kIOReturnSuccess) {
			(void) (*dev)->Release(dev);
			PsychErrMsgTxt("Unable to open device.");
		}
		
		// Configure device
		kr = ConfigureDevice(dev);
		if (kr != kIOReturnSuccess) {
			(void) (*dev)->USBDeviceClose(dev);
			(void) (*dev)->Release(dev);
			PsychErrMsgTxt("Unable to configure device");
		}
	}
	else {
		dev = NULL;
	}
	
	// Finished with master port
	mach_port_deallocate(mach_task_self(), masterPort);
	masterPort = 0;
	
	// Return the pointer to the USB device handle.  This will be NULL if
	// nothing was found.
	return dev;
}


IOReturn ConfigureDevice(IOUSBDeviceInterface **dev)
{
	UInt8                           numConfig;
	IOReturn                        kr;
	IOUSBConfigurationDescriptorPtr configDesc;
	
	// Get the number of configurations. The sample code always chooses
	// the first configuration (at index 0) but your code may need a
	// different one
	kr = (*dev)->GetNumberOfConfigurations(dev, &numConfig);
	
	if (!numConfig) {
		return -1;
	}
	
	// Get the configuration descriptor for index 0
	kr = (*dev)->GetConfigurationDescriptorPtr(dev, 0, &configDesc);
	if (kr) {
		printf("Couldn?t get configuration descriptor for index %d (err = %08x)\n", 0, kr);
		return -1;
	}
	
	// Set the device?s configuration. The configuration value is found in
	// the bConfigurationValue field of the configuration descriptor
	kr = (*dev)->SetConfiguration(dev, configDesc->bConfigurationValue);
	if (kr) {
		printf("Couldn?t set configuration to value %d (err = %08x)\n", 0, kr);
		return -1;
	}
	
	return kIOReturnSuccess;
}
