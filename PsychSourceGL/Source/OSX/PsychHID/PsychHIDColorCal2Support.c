/*
 *  PsychHIDColorCal2OpenDevice.c
 *  PsychToolbox
 *
 *  Platform: OS X
 *
 *  Authors:
 *  Christopher Broussard <chrg@sas.upenn.edu>		cgb
 */

#include "PsychHID.h"

extern IOUSBDeviceInterface **g_ColorCal2Device;

bool PsychHIDColorCal2MakeRequest(psych_uint8 bmRequestType, psych_uint16 wValue, psych_uint16 wIndex, psych_uint16 wLength, void *pData)
{
	bool retVal = true;
	
	// Setup the USB request data structure.
	IOUSBDevRequest request;
	request.bmRequestType = bmRequestType;
	request.wValue = wValue;
	request.wLength = wLength;
	request.wIndex = wIndex;
	
	// Send the data across the USB bus.
	if ((*g_ColorCal2Device)->DeviceRequest(g_ColorCal2Device, &request) != kIOReturnSuccess) {
		retVal = false;
	}
	
	return retVal;
}


bool PSYCHHIDColorCal2OpenDevice(void)
{
	mach_port_t             masterPort;
	kern_return_t           kr;
	CFMutableDictionaryRef  matchingDict;
	SInt32                  usbVendor = kColorCal2VendorID;
	SInt32                  usbProduct = kColorCal2ProductID;
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
		mexErrMsgTxt("(ColorCal) Couldn?t create a master I/O Kit port.");
	}
	
	// Set up matching dictionary for class IOUSBDevice and its subclasses
	matchingDict = IOServiceMatching(kIOUSBDeviceClassName);
	if (!matchingDict) {
		mach_port_deallocate(mach_task_self(), masterPort);
		mexErrMsgTxt("(ColorCal) Couldn?t create a USB matching dictionary\n");
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
		mexErrMsgTxt("(ColorCal) Couldn't get matching services\n");
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
			printf("Couldn?t create a device interface (%08x)\n", (int) result);
			continue;
		}
		
		// Check these values for confirmation.
		kr = (*dev)->GetDeviceVendor(dev, &vendor);
		kr = (*dev)->GetDeviceProduct(dev, &product);
		kr = (*dev)->GetDeviceReleaseNumber(dev, &release);
		if ((vendor != kColorCal2VendorID) || (product != kColorCal2ProductID)) {
			mexPrintf("Found unwanted device (vendor = %d, product = %d)\n", vendor, product);
			(void) (*dev)->Release(dev);
			continue;
		}
		else {
			deviceFound = true;
			//mexPrintf("Vendor: 0x%x\nProduct: 0x%x\nRelease: 0x%x\n", vendor, product, release);
			break;
		}
		
	}
	
	if (deviceFound) {
		// Open the device to change its state
		kr = (*dev)->USBDeviceOpen(dev);
		if (kr != kIOReturnSuccess) {
			(void) (*dev)->Release(dev);
			mexErrMsgTxt("(ColorCal) Unable to open device.");
		}
		
		// Configure device
		kr = ConfigureDevice(dev);
		if (kr != kIOReturnSuccess) {
			(void) (*dev)->USBDeviceClose(dev);
			(void) (*dev)->Release(dev);
			mexErrMsgTxt("Unable to configure device");
		}
		
		// Store a reference to the device.
		g_ColorCal2Device = dev;
	}
	
	// Finished with master port
	mach_port_deallocate(mach_task_self(), masterPort);
	masterPort = 0;
	
	return deviceFound;	
}