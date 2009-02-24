// ColorCal.c
//
// Author: Christopher Broussard
// Date: 2/16/09

#include "ColorCal.h"

// Globals
IOUSBDeviceInterface **ccDevice = NULL;
bool exitFunctionRegistered = false;


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	IOUSBDevRequest request;
	char command[256], buffer[64];
	int i;
	
	// Verify the number of input arguments.
	if (nrhs == 0) {
		mexErrMsgTxt("Usage: ouputData = ColorCal('command', [inputData])");
	}
	
	// Check to see if the first argument is a string.
	if (mxIsChar(prhs[0]) == false) {
		mexErrMsgTxt("First parameter must be a command string.");
	}
	
	// If the device isn't open, go ahead and open it.
	if (ccDevice == NULL) {
		mexPrintf("- Opening ColorCal device...");
		if (OpenDevice() == true) {
			mexPrintf("Done\n");
		}
		else {
			mexErrMsgTxt("(ColorCal) ColorCal2 device not attached to the computer.");
		}
	}
	
	// Make sure the exit function is registered.
	if (exitFunctionRegistered == false) {
		mexAtExit(CloseDevice);
		exitFunctionRegistered = true;
	}
	
	// Extract the command.
	if (mxGetString(prhs[0], command, 256)) {
		mexErrMsgTxt("(ColorCal) Failed to extract command string.");
	}
	
	if (strcasecmp("LEDOn", command) == 0) {
		request.bmRequestType = 0x40;
		request.wValue = 2;
		request.wLength = 0;
		if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
			mexErrMsgTxt("(ColorCal) Failed to turn on LED.");
		}
	}
	else if (strcasecmp("LEDOff", command) == 0) {
		request.bmRequestType = 0x40;
		request.wValue = 3;
		request.wLength = 0;
		if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
			mexErrMsgTxt("(ColorCal) Failed to turn off LED.");
		}
	}
	else if (strcasecmp("GetRawData", command) == 0) {
		UInt32 fatBuffer[7];
		
		// Create the fieldnames for the structure we'll use to return the data.
		char **fieldNames = (char**)mxMalloc(7*sizeof(char*));
		for (i = 0; i < 7; i++) {
			fieldNames[i] = (char*)mxMalloc(16*sizeof(char));
			
			switch (i) {
				case 0:
					strcpy(fieldNames[i], "Xdata");
					break;
				case 1:
					strcpy(fieldNames[i], "Xzero");
					break;
				case 2:
					strcpy(fieldNames[i], "Ydata");
					break;
				case 3:
					strcpy(fieldNames[i], "Yzero");
					break;
				case 4:
					strcpy(fieldNames[i], "Zdata");
					break;
				case 5:
					strcpy(fieldNames[i], "Zzero");
					break;
				case 6:
					strcpy(fieldNames[i], "Trigger");
					break;
			}
		}
		plhs[0] = mxCreateStructMatrix(1, 1, 7, (const char**)fieldNames);

		request.bmRequestType = 0xC0;
		request.wValue = 4;
		request.wLength = 28;
		request.pData = fatBuffer;
		
		// Send read request.
		if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
			mexErrMsgTxt("(ColorCal) Failed to read raw data.");
		}
		
#ifdef __ppc__
		// Swap the byte order of the results on PPC machines.
		for (i = 0; i < 7; i++) {
			fatBuffer[i] = Endian32_Swap(fatBuffer[i]);
		}
#endif

		//mexPrintf("%d\n", fatBuffer[0]);
		
		// Add the results to the struct we'll return to Matlab.
		mxSetField(plhs[0], 0, "Xdata", mxCreateDoubleScalar((double)fatBuffer[0]));
		mxSetField(plhs[0], 0, "Xzero", mxCreateDoubleScalar((double)fatBuffer[1]));
		mxSetField(plhs[0], 0, "Ydata", mxCreateDoubleScalar((double)fatBuffer[2]));
		mxSetField(plhs[0], 0, "Yzero", mxCreateDoubleScalar((double)fatBuffer[3]));
		mxSetField(plhs[0], 0, "Zdata", mxCreateDoubleScalar((double)fatBuffer[4]));
		mxSetField(plhs[0], 0, "Zzero", mxCreateDoubleScalar((double)fatBuffer[5]));
		mxSetField(plhs[0], 0, "Trigger", mxCreateDoubleScalar((double)fatBuffer[6]));
	}
	else if (strcasecmp("MeasureXYZ", command) == 0) {
		float xxx, yyy, zzz;
		request.bmRequestType = 0x40;
		request.wValue = 1;
		request.wLength = 32;
		buffer[0] = 'M'; buffer[1] = 'E'; buffer[2] = 'S';
		request.pData = buffer;
		
		// Send the request to make a measurement.
		if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
			mexErrMsgTxt("(ColorCal) Failed to send measurement request.");
		}
		
		// Grab the result.
		request.bmRequestType = 0xC0;
		if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
			mexErrMsgTxt("(ColorCal) Failed to get measurement.");
		}
		
		// Parse the results.
		sscanf(buffer, "OK00,%6f,%6f,%6f", &xxx, &yyy, &zzz);
		
		// Setup the returned variables.
		plhs[0] = mxCreateDoubleScalar((double)xxx);
		plhs[1] = mxCreateDoubleScalar((double)yyy);
		plhs[2] = mxCreateDoubleScalar((double)zzz);
	}
	else if (strcasecmp("ZeroCalibration", command) == 0) {
		request.bmRequestType = 0x40;
		request.wValue = 1;
		request.wLength = 32;
		buffer[0] = 'U'; buffer[1] = 'Z'; buffer[2] = 'C';
		request.pData = buffer;
		
		// Send the request to zero the calibration.
		if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
			mexErrMsgTxt("(ColorCal) Failed to send zero calibration request.");
		}
		
		// Grab the result.
		request.bmRequestType = 0xC0;
		if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
			mexErrMsgTxt("(ColorCal) Failed to get zero calibration result.");
		}
		
		if (strcmp("OK00\n", buffer) == 0) {
			plhs[0] = mxCreateDoubleScalar(1.0);
		}
		else if (strcmp("ER11\n", buffer) == 0) {
			plhs[0] = mxCreateDoubleScalar(0.0);
		}
		else {
			mexPrintf("* Buffer Data: %s\n", buffer);
			mexErrMsgTxt("(ColorCal) Failed to parse zero calibration return code.");
		}
	}
	else if (strcasecmp("ReadColorMatrix", command) == 0 || strcasecmp("ReadColourMatrix", command) == 0) {
		int xxx, yyy, zzz;
		int row;
		
		// Create the mxArray to hold the data.
		plhs[0] = mxCreateDoubleMatrix(9, 3, mxREAL);
		plhs[1] = mxCreateDoubleMatrix(9, 3, mxREAL);
		double *pr = mxGetPr(plhs[0]);	// Pointer to converted data.
		double *rpr = mxGetPr(plhs[1]); // Pointer to raw data.
		
		for (row = 0; row < 9; row++) {
			request.bmRequestType = 0x40;
			request.wValue = 1;
			request.wLength = 32;
			buffer[0] = 'r'; buffer[1] = '0'; buffer[2] = row +'1';
			request.pData = buffer;
			
			// Send the request to read the matrix.
			if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
				mexErrMsgTxt("(ColorCal) Failed to send read matrix request.");
			}
			
			// Grab the result.
			request.bmRequestType = 0xC0;
			if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
				mexErrMsgTxt("(ColorCal) Failed to read matrix.");
			}
			
			// Parse the results.
			sscanf(buffer, "OK00,%5d,%5d,%5d", &xxx, &yyy, &zzz);
			
			// Converted data.
			pr[row] = (double)DefunnyMatrixValue(xxx);
			pr[row+9] = (double)DefunnyMatrixValue(yyy);
			pr[row+18] = (double)DefunnyMatrixValue(zzz);
			
			// Raw data.
			rpr[row] = (double)xxx;
			rpr[row+9] = (double)yyy;
			rpr[row+18] = (double)zzz;
		}
	}
	else if (strcasecmp("DeviceInfo", command) == 0) {
		int rom_version, build_number, serial_number;
		
		request.bmRequestType = 0x40;
		request.wValue = 1;
		request.wLength = 32;
		buffer[0] = 'I'; buffer[1] = 'D'; buffer[2] = 'R';
		request.pData = buffer;
		
		// Send the request to get the device info.
		if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
			mexErrMsgTxt("(ColorCal) Failed to send device info request.");
		}
		
		// Grab the result.
		request.bmRequestType = 0xC0;
		if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
			mexErrMsgTxt("(ColorCal) Failed to get device info.");
		}
		
		// Parse the results.
		sscanf(buffer, "OK00,1,%3d,100.10,%8d,%3d", &rom_version, &serial_number, &build_number);
		
		// Add the results to Matlab.
		plhs[0] = mxCreateDoubleScalar((double)rom_version);
		plhs[1] = mxCreateDoubleScalar((double)serial_number);
		plhs[2] = mxCreateDoubleScalar((double)build_number);
	}
	else if (strcasecmp("ResetEEProm", command) == 0) {
		request.bmRequestType = 0x40;
		request.wValue = 7;
		request.wLength = 0;
		
		// Send the reset command.
		if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
			mexErrMsgTxt("(ColorCal) Failed to send reset command.");
		}
	}
	else if (strcasecmp("StartBootloader", command) == 0) {
		request.bmRequestType = 0x40;
		request.wValue = 99;
		request.wLength = 0;
		
		// Send the bootloader command.
		if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
			mexErrMsgTxt("(ColorCal) Failed to send bootloader command.");
		}
	}
	else if (strcasecmp("SetTriggerThreshold", command) == 0) {		
		request.bmRequestType = 0x40;
		request.wValue = 8;
		request.wLength = 0;
		int triggerValue;
		
		// Make sure that the user passed a threshold value.
		if (nrhs != 2) {
			mexErrMsgTxt("(ColorCal) Trigger value required.");
		}
		
		// Grab the trigger value.
		triggerValue = (int)mxGetScalar(prhs[1]);
		request.wIndex = (UInt16)triggerValue;
		
		// Send the set trigger threshold command.
		if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
			mexErrMsgTxt("(ColorCal) Failed to set the trigger threshold.");
		}
	}
	else if (strcasecmp("SetLEDFunction", command) == 0) {
		int tValue;
		
		request.bmRequestType = 0x40;
		request.wValue = 9;
		request.wLength = 0;
		
		if (nrhs != 2) {
			mexErrMsgTxt("(ColorCal) LED function value required.");
		}
		
		// Get the LED trigger value and make sure it's valid.
		tValue = (int)mxGetScalar(prhs[1]);
		if (tValue < 0 || tValue > 1) {
			mexErrMsgTxt("(ColorCal) LED function value must be 0 or 1.");
		}
		request.wIndex = (UInt16)tValue;
		
		// Send the new LED function value.
		if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
			mexErrMsgTxt("(ColorCal) Failed to set the LED function value.");
		}
	}
	else if (strcasecmp("SetColorMatrix", command) == 0 || strcasecmp("SetColourMatrix", command) == 0) {
		UInt16 cBuffer[9];
		size_t dims[2];
		double *pr;
		int matrixIndex;
		
		request.bmRequestType = 0x40;
		request.wValue = 6;
		request.wLength = 18;
		request.pData = cBuffer;
		
		// Make sure that the user passed a matrix index and matrix data.
		if (nrhs != 3) {
			mexErrMsgTxt("(ColorCal) Color matrix and index required.");
		}
		
		// Make sure that the matrix index is in the range [0,2].
		matrixIndex = (int)mxGetScalar(prhs[1]);
		if (matrixIndex < 0 || matrixIndex > 2) {
			mexErrMsgTxt("(ColorCal) Matrix index must be in the range [0,2].");
		}
		request.wIndex = (UInt16)matrixIndex;
		
		// Check the matrix dimensions.
		if (mxGetNumberOfDimensions(prhs[2]) != 2) {
			mexErrMsgTxt("(ColorCal) Matrix must only have 2 dimensions.");
		}
		dims[0] = mxGetM(prhs[2]); dims[1] = mxGetN(prhs[2]);
		if (dims[0] != 3 || dims[1] != 3) {
			mexErrMsgTxt("(ColorCal) Matrix must be 3x3.");
		}
		
		// Populate the data phase by iterating over each row.
		pr = mxGetPr(prhs[2]);
		for (i = 0; i < 3; i++) {
			cBuffer[i*3] = RefunnyMatrixValue(pr[i]);
			cBuffer[i*3+1] = RefunnyMatrixValue(pr[i+3]);
			cBuffer[i*3+2] = RefunnyMatrixValue(pr[i+6]);
		}
		
		// Send the new color matrix.
		if ((*ccDevice)->DeviceRequest(ccDevice, &request) != kIOReturnSuccess) {
			mexErrMsgTxt("(ColorCal) Failed to set the color matrix.");
		}
	}
	else {
		mexErrMsgTxt("(ColorCal) Invalid command.");
	}
}


// Converts from floating point to Minolta format.  Performs a byte swap if
// run on a PPC machine.
UInt16 RefunnyMatrixValue(double value)
{
	UInt16 minoltaValue;
	
	value *= 10000.0;
	
	if (value < 0) {
		value *= -1;
		value += 50000.0;
	}
	
	minoltaValue = (UInt16)value;
	
#ifdef __ppc__
	// On PPC machines we need to change the by order from big endian to little.
	minoltaValue = Endian16_Swap(minoltaValue);
#endif
	
	return minoltaValue;
}


// Converts a raw matrix value from Minolta format to floating point.
double DefunnyMatrixValue(int value)
{
	mexPrintf("%d\n", value);
	
	if (value >= 50000) {
		return -((double)value - 50000.0) / 10000.0;
	}
	else {
		return (double)value / 10000.0;
	}
}


static void CloseDevice(void)
{
	if (ccDevice != NULL) {
		mexPrintf("- Closing ColorCal Device\n");
		(void)(*ccDevice)->USBDeviceClose(ccDevice);
		(void)(*ccDevice)->Release(ccDevice);
		ccDevice = NULL;
	}
}


bool OpenDevice(void)
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
		ccDevice = dev;
	}
	
	// Finished with master port
	mach_port_deallocate(mach_task_self(), masterPort);
	masterPort = 0;
	
	return deviceFound;
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
