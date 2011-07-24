/*
 *  PsychSourceGL/Source/Common/PsychHID/PsychHIDGenericUSBLibSupport.c
 *
 *  PROJECTS: PsychHID
 *
 *  Platform: Linux, Windows
 *
 *  Authors:
 *
 *  mario.kleiner@tuebingen.mpg.de    mk
 *
 *  HISTORY:
 *
 *	22.7.2011	Created.
 *
 *  DESCRIPTION:
 *
 *  Support routines that implement generic USB device handling via libusb-1.0.
 *  Currently used to implement PsychHID support on Linux and MS-Windows.
 *
 */

#include "PsychHID.h"

// Non OS/X only, for now:
#if PSYCH_SYSTEM != PSYCH_OSX

// Function Declarations
int ConfigureDevice(libusb_device_handle* dev, int configIdx);

// Perform device control transfer on USB device:
int PsychHIDOSControlTransfer(PsychUSBDeviceRecord* devRecord, psych_uint8 bmRequestType, psych_uint8 bRequest, psych_uint16 wValue, psych_uint16 wIndex, psych_uint16 wLength, void *pData)
{
    int rc;
	libusb_device_handle* dev = devRecord->device;
    
	if (dev == NULL) {
		PsychErrorExitMsg(PsychError_internal, "libusb_device_handle* device points to NULL device!");
	}

	// Send the data across the USB bus by executing the device control request. Return status code.
    // We use a timeout value of 10000 msecs, aka 10 seconds for request to complete:
    rc = libusb_control_transfer(dev, bmRequestType, bRequest, wValue, wIndex, (unsigned char*) pData, wLength, 10000);

    // Return value is either number of transmitted bytes, or a negative error code.
    // We map any non-error return to zero, as on OS/X:
    if (rc >= 0) return(0);
    
    // Failure! Map to verbose message:
    // TODO...
    
    return (rc);
}

// Close USB device, mark device record as "free/invalid":
void PsychHIDOSCloseUSBDevice(PsychUSBDeviceRecord* devRecord)
{	
    libusb_close((libusb_device_handle*) devRecord->device);
	devRecord->device = NULL;
	devRecord->valid = 0;
}

// Open first USB device that satisfies given matching critera, mark device record as "active/valid":
// errorcode would contain a diagnostic error code on failure, but is not yet used.
// spec contains the specification of the device to open and how to configure it at open time.
// Returns true on success, false on error or if no matching device could be found.
psych_bool PsychHIDOSOpenUSBDevice(PsychUSBDeviceRecord* devRecord, int* errorcode, PsychUSBSetupSpec* spec)
{
    int                     rc;
	psych_uint16            usbVendor  = (psych_uint16) spec->vendorID;
	psych_uint16            usbProduct = (psych_uint16) spec->deviceID;
	libusb_device_handle*   dev = NULL;
    psych_bool              deviceFound = FALSE;
    
    dev = libusb_open_device_with_vid_pid(NULL, usbVendor, usbProduct);
    if (dev) {	
        // Got it!
        deviceFound = TRUE;
        
		// Success! Assign device interface and mark device record as active/open/valid:
		devRecord->device = dev;
		devRecord->valid = 1;

		// Configure device
		rc = ConfigureDevice(dev, spec->configurationID);
		if (rc != 0) {
            // Failed! Close device again and zero-out data structures:
            PsychHIDOSCloseUSBDevice(devRecord);
            
			*errorcode = rc;
			printf("PsychHID-ERROR: Unable to configure USB device during Open for configuration id %i.\n", spec->configurationID);
            return(FALSE);
		}

		// Set errorcode to success:
		*errorcode = 0;
	}
	else {
		// No matching device found. NULL-out the record, we're done.
		// This is not strictly needed, as this NULL state is the initial
		// state of the record upon entering this function.
		devRecord->device = NULL;
		devRecord->valid = 0;
		*errorcode = -1;
	}

	// Return the success status.
	return(deviceFound);
}

int ConfigureDevice(libusb_device_handle* dev, int configIdx)
{
	psych_uint8     numConfig;
    libusb_device*  usbdev;
    struct libusb_device_descriptor deviceDesc;
	struct libusb_config_descriptor* configDesc;
    psych_uint8     bConfigurationValue;
	int rc;

    // Get device pointer for handle:
    usbdev = libusb_get_device(dev);

	// Get the number of configurations. The sample code always chooses
	// the first configuration (at index 0) but your code may need a
	// different one
    rc = libusb_get_device_descriptor(usbdev, &deviceDesc);
    numConfig = deviceDesc.bNumConfigurations;

	if (rc || (numConfig == 0)) {
		printf("PsychHID: USB ConfigureDevice: ERROR! Error getting number of configurations or no configurations available at all (err = %08x)\n", rc);
		return(rc);
	}

	if (configIdx < 0 || configIdx >= (int) numConfig) {
		printf("PsychHID: USB ConfigureDevice: ERROR! Provided configuration index %i outside support range 0 - %i for this device!\n", configIdx, (int) numConfig);
		return(-1);
	}
	
	// Get the configuration descriptor for index 'configIdx':
    rc = libusb_get_config_descriptor(usbdev, (psych_uint8) configIdx, &configDesc);    	
	if (rc) {
		printf("PsychHID: USB ConfigureDevice: ERROR! Couldn't get configuration descriptor for index %d (err = %08x)\n", configIdx, rc);
		return(rc);
	}
	
    // Extract bConfigurationValue:
    bConfigurationValue = configDesc->bConfigurationValue;
    
    // Release descriptor:
    libusb_free_config_descriptor(configDesc);
    
	// Set the device's configuration. The configuration value is found in
	// the bConfigurationValue field of the configuration descriptor
    rc = libusb_set_configuration(dev, (int) bConfigurationValue);
	if (rc) {
		printf("PsychHID: USB ConfigureDevice: ERROR! Couldn't set configuration to value %d (err = %08x)\n", (int) bConfigurationValue, rc);
		return(rc);
	}
	
    // Return success:
	return(0);
}

#endif
