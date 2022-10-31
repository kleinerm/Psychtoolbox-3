/*
 *  PsychSourceGL/Source/Common/PsychHID/PsychHIDGenericUSBLibSupport.c
 *
 *  PROJECTS: PsychHID
 *
 *  Platform: All supported by libusb-1.0.
 *
 *  Authors:
 *
 *  mario.kleiner.de@gmail.com  mk
 *
 *  HISTORY:
 *
 *  22.07.2011  Created.
 *  25.10.2022  Extended to support macOS.
 *
 *  DESCRIPTION:
 *
 *  Support routines that implement generic USB device handling via libusb-1.0.
 *  Currently used to implement PsychHID support on Linux and MS-Windows and macOS.
 *
 */

#include "PsychHID.h"

#if PSYCH_SYSTEM == PSYCH_WINDOWS
#pragma warning( disable : 4200 )
#endif

// Master include file for libusb:
// CAUTION: This #undef'ines the "interface" keyword on MS-Windows, which
// will cause breakage with DirectX! Therefore this *must* be defined
// *after* include's for dinput.h !!
#include <libusb.h>

// Define simple replacement for libusb_error_name() when building against
// old libusb versions older than v1.0.9:
#if !defined(LIBUSB_API_VERSION)
static const char* libusb_error_name(int errcode)
{
    static char errcodestr[11];
    sprintf(errcodestr, "Error %i", errcode);
    return(errcodestr);
}
#endif

#if PSYCH_SYSTEM == PSYCH_OSX
extern int libusb_init(libusb_context **ctx) __attribute__((weak_import));
#endif

static int ctx_refcount = 0;
static libusb_context *ctx = NULL;

// Function Declarations
int ConfigureDevice(libusb_device_handle* dev, int configIdx);

// Perform device control transfer on USB device:
int PsychHIDOSControlTransfer(PsychUSBDeviceRecord* devRecord, psych_uint8 bmRequestType, psych_uint8 bRequest,
                              psych_uint16 wValue, psych_uint16 wIndex, psych_uint16 wLength, void *pData, unsigned int timeOutMSecs)
{
    int rc;
    libusb_device_handle* dev = (libusb_device_handle*) devRecord->device;

    if (dev == NULL)
        PsychErrorExitMsg(PsychError_internal, "libusb_device_handle* device points to NULL device!");

    rc = libusb_control_transfer(dev, bmRequestType, bRequest, wValue, wIndex, (unsigned char*) pData, wLength, timeOutMSecs);

    // Return value is either number of transmitted bytes, or a negative error code.
    if (rc < 0)
        printf("PsychHID-ERROR: USB control transfer failed: %s - %s.\n", libusb_error_name(rc), libusb_strerror(rc));

    return (rc);
}

// Close USB device, mark device record as "free/invalid":
void PsychHIDOSCloseUSBDevice(PsychUSBDeviceRecord* devRecord)
{
    libusb_close((libusb_device_handle*) devRecord->device);
    devRecord->device = NULL;
    devRecord->valid = 0;

    ctx_refcount--;

    if (ctx_refcount == 0) {
        libusb_exit(ctx);
        ctx = NULL;
    }
}

// Open first USB device that satisfies given matching critera, mark device record as "active/valid":
// errorcode would contain a diagnostic error code on failure, but is not yet used.
// spec contains the specification of the device to open and how to configure it at open time.
// Returns true on success, false on error or if no matching device could be found.
psych_bool PsychHIDOSOpenUSBDevice(PsychUSBDeviceRecord* devRecord, int* errorcode, PsychUSBSetupSpec* spec)
{
    int                     rc;
    psych_uint16            usbVendor;
    psych_uint16            usbProduct;
    libusb_device_handle*   dev = NULL;
    psych_bool              deviceFound = FALSE;

    // Just a query if USB access is supported?
    if (devRecord == NULL) {
        // Yes. It is always supported on Linux and Windows, if we made it to this point.
        // On macOS we need to check if libusb-1.dylib was successfully loaded and weak_linked:
        #if PSYCH_SYSTEM == PSYCH_OSX
        return(NULL != libusb_init);
        #endif

        // Linux or Windows - always ok:
        return(TRUE);
    }

    // Wants to really open a USB device:
    usbVendor  = (psych_uint16) spec->vendorID;
    usbProduct = (psych_uint16) spec->deviceID;

    if (NULL == ctx) {
        // Check if linker was able to dynamically runtime-link the
        // libusb 1 library on macOS, where we weak-link the library to
        // allow operation of PsychHID() without need to have libusb
        // installed, as long as user doesn't want to use low-level USB:
        #if PSYCH_SYSTEM == PSYCH_OSX
            if (NULL == libusb_init) {
                printf("\n\n");
                printf("PTB-ERROR: Could not load and link libusb.dylib low-level USB access library!\n");
                printf("PTB-ERROR: Most likely because the library is not (properly) installed on this\n");
                printf("PTB-ERROR: machine. Please read 'help PsychHID' for installation or\n");
                printf("PTB-ERROR: troubleshooting instructions. Low level USB support is disabled\n");
                printf("PTB-ERROR: until you have resolved the problem.\n\n");
                PsychErrorExitMsg(PsychError_user, "Failed to load and link libusb.dylib low-level USB access library! Low level USB access unavailable.");
            }
        #endif

        rc = libusb_init(&ctx);
        if (rc) {
            *errorcode = rc;
            printf("PTB-ERROR: Could not initialize libusb-1 low-level USB access library: %s - %s.\n", libusb_error_name(rc), libusb_strerror(rc));
            return(FALSE);
        }

        // Set level of verbosity. Use new api since libusb version 1.0.22:
        #if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x01000106)
            libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING);
        #else
            libusb_set_debug(ctx, 3);
        #endif
    }

    dev = libusb_open_device_with_vid_pid(ctx, usbVendor, usbProduct);
    if (dev) {
        // Got it!

        // Increment refcount of libusb users:
        ctx_refcount++;

        // Success! Assign device interface and mark device record as active/open/valid:
        devRecord->device = (void*) dev;
        devRecord->valid = 1;

        // Configure device
        rc = ConfigureDevice(dev, spec->configurationID);
        if (rc != 0) {
            // Failed! Close device again and zero-out data structures:
            PsychHIDOSCloseUSBDevice(devRecord);

            *errorcode = rc;
            printf("PsychHID-ERROR: Unable to configure USB device during Open for configuration id %i: %s - %s.\n", spec->configurationID, libusb_error_name(rc), libusb_strerror(rc));

            return(FALSE);
        }

        // Set errorcode to success:
        *errorcode = LIBUSB_SUCCESS;
        deviceFound = TRUE;
    }
    else {
        // No matching device found. NULL-out the record, we're done.
        // This is not strictly needed, as this NULL state is the initial
        // state of the record upon entering this function.
        devRecord->device = NULL;
        devRecord->valid = 0;
        *errorcode = LIBUSB_ERROR_NO_DEVICE;
    }

    if (ctx_refcount == 0) {
        libusb_exit(ctx);
        ctx = NULL;
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
    int bConfigurationValue, current_bConfigurationValue;
    int rc;

    // A configIdx == -1 means: Skip configuration.
    if (configIdx == -1) return(LIBUSB_SUCCESS);

    // Get device pointer for handle:
    usbdev = libusb_get_device(dev);

    // Get the number of configurations. The sample code always chooses
    // the first configuration (at index 0) but your code may need a
    // different one
    rc = libusb_get_device_descriptor(usbdev, &deviceDesc);
    numConfig = deviceDesc.bNumConfigurations;

    if (rc || (numConfig == 0)) {
        printf("PsychHID: USB ConfigureDevice: ERROR! Error getting number of device configurations, or no configurations available at all (err = %d)\n", rc);
        return(rc);
    }

    if (configIdx < 0 || configIdx >= (int) numConfig) {
        printf("PsychHID: USB ConfigureDevice: ERROR! Provided configuration index %i outside support range 0 - %i for this device!\n", configIdx, (int) numConfig - 1);
        return(LIBUSB_ERROR_INVALID_PARAM);
    }

    // Get the configuration descriptor for index 'configIdx':
    rc = libusb_get_config_descriptor(usbdev, (psych_uint8) configIdx, &configDesc);
    if (rc) {
        printf("PsychHID: USB ConfigureDevice: ERROR! Couldn't get configuration descriptor for index %d (err = %d)\n", configIdx, rc);
        return(rc);
    }

    // Extract bConfigurationValue:
    bConfigurationValue = (int) configDesc->bConfigurationValue;

    // Release descriptor:
    libusb_free_config_descriptor(configDesc);

    rc = libusb_get_configuration(dev, &current_bConfigurationValue);
    if (rc) {
        printf("PsychHID: USB ConfigureDevice: ERROR! Couldn't get current configuration of device (err = %d)\n", rc);
        return(rc);
    }

    // If current value is already identical to requested value, we're done:
    if (current_bConfigurationValue == bConfigurationValue) return(LIBUSB_SUCCESS);

    // Set the device's configuration. The configuration value is found in
    // the bConfigurationValue field of the configuration descriptor
    rc = libusb_set_configuration(dev, bConfigurationValue);
    if (rc) {
        printf("PsychHID: USB ConfigureDevice: ERROR! Couldn't set configuration to value %d (err = %d)\n", bConfigurationValue, rc);
        return(rc);
    }

    // Return success:
    return(LIBUSB_SUCCESS);
}

int PsychHIDOSBulkTransfer(PsychUSBDeviceRecord* devRecord, psych_uint8 endPoint, int length, psych_uint8* buffer, int* count, unsigned int timeOutMSecs)
{
    int rc;
    libusb_device_handle* dev = (libusb_device_handle*) devRecord->device;

    if (dev == NULL)
        PsychErrorExitMsg(PsychError_internal, "libusb_device_handle* device points to NULL device!");

    rc = libusb_bulk_transfer(dev, (unsigned char) endPoint, (unsigned char*) buffer, length, count, timeOutMSecs);

    // Return value is either 0 on success, or a negative error code.
    if (rc < 0)
        printf("PsychHID-ERROR: USB bulk transfer failed: %s - %s.\n", libusb_error_name(rc), libusb_strerror(rc));

    return (rc);
}

int PsychHIDOSInterruptTransfer(PsychUSBDeviceRecord* devRecord, psych_uint8 endPoint, int length, psych_uint8* buffer, int* count, unsigned int timeOutMSecs)
{
    int rc;
    libusb_device_handle* dev = (libusb_device_handle*) devRecord->device;

    if (dev == NULL)
        PsychErrorExitMsg(PsychError_internal, "libusb_device_handle* device points to NULL device!");

    rc = libusb_interrupt_transfer(dev, (unsigned char) endPoint, (unsigned char*) buffer, length, count, timeOutMSecs);

    // Return value is either 0 on success, or a negative error code.
    if (rc < 0)
        printf("PsychHID-ERROR: USB interrupt transfer failed: %s - %s.\n", libusb_error_name(rc), libusb_strerror(rc));

    return (rc);
}
