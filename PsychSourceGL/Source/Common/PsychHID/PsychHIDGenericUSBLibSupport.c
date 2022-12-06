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
        devRecord->firstClaimedInterface = -1;

        // Configure device
        rc = ConfigureDevice(dev, spec->configurationID);
        if (rc != 0) {
            // Failed! Close device again and zero-out data structures:
            PsychHIDOSCloseUSBDevice(devRecord);

            *errorcode = rc;
            printf("PsychHID-ERROR: Unable to configure USB device during Open for configuration id %i: %s - %s.\n", spec->configurationID, libusb_error_name(rc), libusb_strerror(rc));

            return(FALSE);
        }

        // LIBUSB_API_VERSION introduced in v1.0.18, following function requires 1.0.16+
        #ifdef LIBUSB_API_VERSION
            // On supported platforms, otherwise does nothing: Enable automatic detaching of
            // kernel drivers when a specific interface is claimed, and reattaching of formerly
            // detached kernel drivers when the interface is released again:
            rc = libusb_set_auto_detach_kernel_driver(dev, 1);
            if ((rc != LIBUSB_SUCCESS) && (rc != LIBUSB_ERROR_NOT_SUPPORTED))
                printf("PsychHID-ERROR: Unable to enable automatic detaching of kernel drivers: %s - %s.\n", libusb_error_name(rc), libusb_strerror(rc));
        #endif

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

    // If no interface was claimed by user script yet, try to claim interface #0 to enable this transfer:
    if ((devRecord->firstClaimedInterface < 0) && ((rc = PsychHIDOSClaimInterface(devRecord, 0)) < 0))
        return (rc);

    // Execute bulk transfer:
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

    // If no interface was claimed by user script yet, try to claim interface #0 to enable this transfer:
    if ((devRecord->firstClaimedInterface < 0) && ((rc = PsychHIDOSClaimInterface(devRecord, 0)) < 0))
        return (rc);

    // Execute interrupt transfer:
    rc = libusb_interrupt_transfer(dev, (unsigned char) endPoint, (unsigned char*) buffer, length, count, timeOutMSecs);

    // Return value is either 0 on success, or a negative error code.
    if (rc < 0)
        printf("PsychHID-ERROR: USB interrupt transfer failed: %s - %s.\n", libusb_error_name(rc), libusb_strerror(rc));

    return (rc);
}

int PsychHIDOSClaimInterface(PsychUSBDeviceRecord* devRecord, int interfaceId)
{
    int rc;
    libusb_device_handle* dev = (libusb_device_handle*) devRecord->device;

    if (dev == NULL)
        PsychErrorExitMsg(PsychError_internal, "libusb_device_handle* device points to NULL device!");

    // Try to claim interface interfaceId:
    rc = libusb_claim_interface(dev, interfaceId);

    #ifdef LIBUSB_API_VERSION
    if (rc == LIBUSB_ERROR_BUSY) {
        // Disable auto-detach of kernel drivers on claim interface, maybe this will go better then:
        rc = libusb_set_auto_detach_kernel_driver(dev, 0);
        if ((rc != LIBUSB_SUCCESS) && (rc != LIBUSB_ERROR_NOT_SUPPORTED)) {
            printf("PsychHID-ERROR: Unable to disable automatic detaching of kernel drivers: %s - %s.\n", libusb_error_name(rc), libusb_strerror(rc));
            rc = LIBUSB_ERROR_BUSY;
        }
        else {
            if (libusb_kernel_driver_active(dev, interfaceId) == 1)
                printf("PsychHID-WARNING: Non-usbfs kernel driver attached to interface %i after libusb_claim_interface() with auto-detach?!? This spells trouble!\n",
                       interfaceId);

            if (libusb_kernel_driver_active(dev, interfaceId) == 0) {
                printf("PsychHID-WARNING: No kernel driver attached, or usbfs kernel driver attached, to interface %i after libusb_claim_interface() with auto-detach!\n",
                       interfaceId);
                printf("PsychHID-WARNING: This is known to happen if the calling (user) script has a certain bug. Specifically if the\n");
                printf("PsychHID-WARNING: script called PsychHID('USBControlTransfer', ...) on an USB endpoint other than endpoint zero,\n");
                printf("PsychHID-WARNING: or directly on a interface, *and* it didn't first manually claim the USB interface (associated with\n");
                printf("PsychHID-WARNING: that endpoint if any), as required by USB spec, and the associated interface did not have a kernel\n");
                printf("PsychHID-WARNING: driver attached already. At least the Linux kernel (maybe also other operating systems?) would try\n");
                printf("PsychHID-WARNING: to fix/workaround this mistake by auto-claiming the interface. This may work for the control transfer,\n");
                printf("PsychHID-WARNING: but it can cause successive failure, if the script afterwards claims the same interface manually via\n");
                printf("PsychHID-WARNING: PsychHID('USBClaimInterface', ...), or indirectly via attempting a bulk-/interrupt-/iso-transfer.\n");
                printf("PsychHID-WARNING: I will try to fix this problem now, which may or may not work to keep your script going. We'll see...\n");
                printf("PsychHID-WARNING: However, please fix the offending user script properly by explicitely claiming the proper interface\n");
                printf("PsychHID-WARNING: before issuing a control transfer to such a non-zero endpoint or to an interface directly.\n");
                printf("PsychHID-WARNING: Note: bmRequestType bits 0-4 select the recipient, wIndex defines the endpoint address or interface.\n");
            }

            // Try to manually detach driver other than usbfs, if any. This is unlikely to work if the auto-detach
            // failed already, but let us try it anyway. There are no known cases where this helps, but also none
            // where it would make things worse, iow. it is useless at worst, but who knows, maybe there is some
            // special edge case where it would actually help? Courage!
            rc = libusb_detach_kernel_driver(dev, interfaceId);
            if ((rc != 0) && (rc != LIBUSB_ERROR_NOT_FOUND))
                printf("PsychHID-WARNING: Could not manually detach non-usbfs kernel driver from interface %i, this will probably go badly! %s - %s.\n",
                       interfaceId, libusb_error_name(rc), libusb_strerror(rc));

            // Retry to claim interface interfaceId, this time without auto-detaching driver:
            // This can work in a scenario where the Linux kernel itself has auto-claimed the interface interfaceId
            // on our behalf and attached the usbfs kernel driver to it, but without libusb noticing this, ie. an
            // "accounting error", where libusb's view of the world is mismatched to the actual reality in the kernel.
            //
            // Such a scenario can happen if a USB control transfer from/to an endpoint other than EP 0 was attempted by a
            // deficient/buggy user-script without claiming the associated interface, as required, or a control transfer
            // from/to an interface was attempted without claiming the interface, and all this was done on a
            // interface without any kernel driver attached, as the Linux kernel would try to work around the
            // deficient user-script by auto-claiming that interface on our behalf, but "behind libusb's back", with the
            // result that such scripts with their USB control transfers would work in many cases, but claiming the
            // affected (already auto-claimed) interface would fail due to libusb accounting mismatch. E.g., such an
            // error case would be a control transfer to non-zero endpoint (wrong wrong wrong!), fixed by the kernel,
            // (cfe. https://elixir.bootlin.com/linux/v5.15.81/source/drivers/usb/core/devio.c#L817 checkintf() function
            // for Linux 5.15 as shipping in Ubuntu 20.04.5-LTS or Ubuntu 22.04-LTS for the auto-claiming, triggered by
            // libusb_submit_transfer() -> proc_do_submiturb() when detecting a non-standard control transfer endpoint
            // by the logic in https://elixir.bootlin.com/linux/v5.15.81/source/drivers/usb/core/devio.c#L1627 and
            // https://elixir.bootlin.com/linux/v5.15.81/source/drivers/usb/core/devio.c#L1660 ie. the called subroutine
            // check_ctrlrecip() at the bottom: https://elixir.bootlin.com/linux/v5.15.81/source/drivers/usb/core/devio.c#L884 )
            //
            // ... now working, but then followed by a bulk or interrupt transfer trying to claim the interface, or explicit
            // claiming of the interface -> Accounting mismatch makes libusb_claim_interface() in auto-detach-kernel-driver
            // mode fail, as it asks the kernel to detach any kernel driver except usbfs, and the kernel would error
            // reject that request due to the already bound (on our behalf, behind our back, as part of auto-claim)
            // usbfs driver. Cfe. libusb would use the autodetach ioctl(), as implemented in proc_disconnect_claim()
            // at https://elixir.bootlin.com/linux/v5.15.81/source/drivers/usb/core/devio.c#L2439 , specifically the
            // "if ((dc.flags & USBDEVFS_DISCONNECT_CLAIM_EXCEPT_DRIVER) ...) return -EBUSY;" path will trigger in
            // such a case due to bound usbfs kernel driver.
            //
            // Repeating libusb_claim_interace() with auto-detach-kernel-driver disabled, will just try to claim the
            // interface again via a standard claim interface ioctl(). That will succeed as a "no-op success return" as
            // the kernel knows the interface is already claimed for us, so reports success. Cfe. Linux 5.15 claimintf(),
            // at https://elixir.bootlin.com/linux/v5.15.81/source/drivers/usb/core/devio.c#L758 "if (testbit...) return 0"...
            //
            // This will cause libusb to be pleased and update its accounting to match reality and all is good.
            // However, in cases where a different kernel driver than usbfs was attached during the erroneous USB
            // control transfer, the kernel could not fix this automagically and the USB control transfer would fail
            // with LIBUSB_ERROR_IO, so this only helps in a limited number of cases. But no harm trying, and our
            // diagnostic code above should be able to give some warning that points to the broken user script and
            // a potential way to fix it:
            rc = libusb_claim_interface(dev, interfaceId);

            // Reenable driver auto-detach for future claim interface invocations:
            libusb_set_auto_detach_kernel_driver(dev, 1);
        }
    }
    #endif

    // Return value is either 0 on success, or a negative error code.
    if (rc < 0) {
        printf("PsychHID-ERROR: Claiming USB interface %i failed: %s - %s.\n", interfaceId, libusb_error_name(rc), libusb_strerror(rc));

        // Busy? Could mean a kernel driver or a userspace driver/app is claiming/blocking the interface. Let's see:
        if (rc == LIBUSB_ERROR_BUSY) {
            rc = libusb_kernel_driver_active(dev, interfaceId);
            switch (rc) {
                case 0: // No kernel driver active -> Userspace software claimed device?
                    printf("PsychHID-ERROR: Interface %i does not have any kernel drivers attached. Check if some other application is using the device already.\n",
                           interfaceId);
                break;

                case 1: // Kernel driver active -> Auto-detach failed?
                    printf("PsychHID-ERROR: Interface %i does have a kernel driver attached, and auto-detach failed! Try to manually disable/detach that kernel driver.\n",
                           interfaceId);
                break;

                case LIBUSB_ERROR_NOT_SUPPORTED: // Query not supported. Nothing to do here, no point in making noise.
                break;

                default:
                    printf("PsychHID-ERROR: Unable to detect status of kernel drivers on interface %i: %s - %s.\n", interfaceId, libusb_error_name(rc), libusb_strerror(rc));
            }

            // Restore error code for claim interface:
            rc = LIBUSB_ERROR_BUSY;
        }
    } else if (devRecord->firstClaimedInterface < 0) {
        // If no interface was claimed yet, record this as the first claimed interface:
        devRecord->firstClaimedInterface = interfaceId;
    }

    return (rc);
}

