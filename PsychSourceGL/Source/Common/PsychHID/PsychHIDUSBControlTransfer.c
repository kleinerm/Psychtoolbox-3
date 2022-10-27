/*
 * PsychSourceGL/Source/Common/PsychHID/PsychHIDUSBControlTransfer.c
 *
 * PROJECTS: PsychHID
 *
 * PLATFORMS:   All.
 *
 * AUTHORS:
 *
 * chrg@sas.upenn.edu           cgb
 * mario.kleiner.de@gmail.com   mk
 *
 * HISTORY:
 *
 * 4.4.2009     Created.
 */

#include "PsychHID.h"
const int USB_INTRANSFER = 0x80;   // bmRequestType & USB_INTRANSFER or bEndpointAddress & USB_INTRANSFER -> Receive data from device.

static char useString[] = "[recData, count] = PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength [, outData][, timeOutMSecs=10000])";
//                          1        2                                       1          2              3         4       5       6          7          8
static char synopsisString[] =  "Communicates with a USB device via the control endpoint, also known as a control transfer.\n"
                                "The results of in-transfers are returned in the 1st return argument 'recData' as a uint8 array "
                                "of length 'wLength'. The amount of actually received data can be less than 'wLength' and is "
                                "returned in the 2nd return argument 'count'. Therefore, in case of short in-transfers, 'count' "
                                "can be less than 'wLength' (which is also the length of 'recData' (iow. length(recData)).\n"
                                "In case of an out-transfer, 'recData' does not exist, and 'count' will instead be the 1st and "
                                "only return argument, reporting the amount of actual data sent to the device. 'outData' must be "
                                "a uint8() array of at least 'wLength' bytes for out-transfers, but all bytes beyond the first "
                                "'wLength' bytes will be ignored.\n"
                                "The actual number of bytes transfered in any direction is returned in return argument 'count'.\n"
                                "'usbHandle' is the handle of the USB device to control.\n"
                                "'bmRequestType' is the type of request, an 8 bit bit-mask: If bit 7 is set, this defines an in-transfer "
                                "from device to host and 'recData' will be filled with at most 'wLength' bytes received from the device. "
                                "Otherwise it defines a transfer from host to device and at most 'wLength' bytes will be transferred from "
                                "'outData' to the device.\n"
                                "'bRequest' is the request id.\n"
                                "'wValue' and 'wIndex' are device- and request specific values.\n"
                                "'wLength' is the amount of data to return at most on an in-transfer, or the amount of data to send at "
                                "most for an out-transfer in the optional uint8 vector 'outData'. 'outData' must have at least as many "
                                "elements as the value of 'wLength'!\n"
                                "'timeOutMSecs' is an optional timeout for the operation, in milliseconds. Default is 10000 msecs. "
                                "A value of zero means to never time out, but wait indefinitely.\n";
static char seeAlsoString[] =   "OpenUSBDevice USBBulkTransfer USBInterruptTransfer";

PsychError PSYCHHIDUSBControlTransfer(void)
{
    PsychUSBDeviceRecord *dev;
    int usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength;
    int m, n, p, err;
    psych_uint8 *buffer = NULL;
    int timeOutMSecs = 10000;

    // Setup the help features.
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return PsychError_none; }

    // Make sure the correct number of input arguments is supplied:
    PsychErrorExit(PsychRequireNumInputArgs(6));
    PsychErrorExit(PsychCapNumInputArgs(8));
    PsychErrorExit(PsychCapNumOutputArgs(2));

    // Copy all input values. The input data is interpreted as a byte array:
    PsychCopyInIntegerArg(1, TRUE, &usbHandle);
    PsychCopyInIntegerArg(2, TRUE, &bmRequestType);
    PsychCopyInIntegerArg(3, TRUE, &bRequest);
    PsychCopyInIntegerArg(4, TRUE, &wValue);
    PsychCopyInIntegerArg(5, TRUE, &wIndex);
    PsychCopyInIntegerArg(6, TRUE, &wLength);

    // Get 'dev'icerecord for handle: This will error-out if no such device open:
    dev = PsychHIDGetUSBDevice(usbHandle);

    // IN-Transfer for reception of data from USB device?
    if (bmRequestType & USB_INTRANSFER) {
        if (wLength <= 0) {
            PsychErrorExitMsg(PsychError_user, "Argument wLength must be > 0 for an in-transfer command!");
        }

        // Allocate return buffer of sufficient size wLength:
        m = 1; n = wLength; p = 1;
        PsychAllocOutUnsignedByteMatArg(1, TRUE, m, n, p, &buffer);
    }
    else {
        // Get the outData buffer if it was specified.
        m = n = p = 0;
        PsychAllocInUnsignedByteMatArg(7, FALSE, &m, &n, &p, &buffer);
        if (((m * n) > 0) && (p != 1))
            PsychErrorExitMsg(PsychError_user, "Argument outData must be a 1D vector or 2D matrix of bytes! This is a 3D matrix!");

        // Is the outData buffer at least as big as the provided wLength argument?
        if ((m * n) < wLength)
            PsychErrorExitMsg(PsychError_user, "Argument outData is missing or has less elements than provided wLength argument! This is too little!");
    }

    // Timeout provided? Get it and validate:
    if (PsychCopyInIntegerArg(8, FALSE, &timeOutMSecs) && (timeOutMSecs < 0))
        PsychErrorExitMsg(PsychError_user, "Argument timeOutMSecs is negative, but must be at least 0 milliseconds!");

    // Make the actual control transfer request:
    if ((err = PsychHIDOSControlTransfer(dev, (psych_uint8) bmRequestType, (psych_uint8) bRequest, (psych_uint16) wValue,
                                         (psych_uint16) wIndex, (psych_uint16) wLength, (void*) buffer, (unsigned int) timeOutMSecs)) < 0)
        PsychErrorExitMsg(PsychError_system, "The USB Control transfer failed.");

    // Return err - the number of actually transfered bytes:
    PsychCopyOutDoubleArg((bmRequestType & USB_INTRANSFER) ? 2 : 1, FALSE, (double) err);

    return(PsychError_none);
}

PsychError PSYCHHIDUSBBulkTransfer(void)
{
    static char useString[] = "[count, recData] = PsychHID('USBBulkTransfer', usbHandle, endPoint, length [, outData][, timeOutMSecs=10000])";
    //                          1      2                                      1          2         3         4          5
    static char synopsisString[] =  "Performs a USB bulk transfer.\n"
    "The results of in-transfers are returned in the 2nd return argument 'recData' as a uint8 array "
    "of length 'length'. The amount of actually received data can be less than 'length' and is "
    "returned in the return argument 'count'. Therefore, in case of short in-transfers, 'count' "
    "can be less than 'length' (which is also the length of 'recData' (iow. length(recData)).\n"
    "In case of an out-transfer, 'recData' does not exist, and 'count' will instead be the "
    "only return argument, reporting the amount of actual data sent to the device. 'outData' must be "
    "a uint8() array of at least 'length' bytes for out-transfers, but all bytes beyond the first "
    "'length' bytes will be ignored.\n"
    "The actual number of bytes transfered in any direction is returned in return argument 'count'.\n"
    "'usbHandle' is the handle of the USB device to perform the transfer on.\n"
    "'endPoint' is the end-point address: If bit 7 is set, this defines a bulk in-transfer from "
    "device to host and 'recData' will be filled with at most 'length' bytes received from the device. "
    "Otherwise it defines a bulk out-transfer from host to device and at most 'length' bytes will be "
    "transferred from 'outData' to the device.\n"
    "'length' is the amount of data to return at most on an in-transfer, or the amount of data to send at "
    "most for an out-transfer in the optional uint8 vector 'outData'. 'outData' must have at least as many "
    "elements as the value of 'length'!\n"
    "'timeOutMSecs' is an optional timeout for the operation, in milliseconds. Default is 10000 msecs. "
    "A value of zero means to never time out, but wait indefinitely.\n";
    static char seeAlsoString[] =   "OpenUSBDevice USBControlTransfer USBInterruptTransfer";

    PsychUSBDeviceRecord *dev;
    int usbHandle, endPoint, length, count;
    int m, n, p, err;
    psych_uint8 *buffer = NULL;
    int timeOutMSecs = 10000;

    // Setup the help features.
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return PsychError_none; }

    // Make sure the correct number of input arguments is supplied:
    PsychErrorExit(PsychRequireNumInputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(5));
    PsychErrorExit(PsychCapNumOutputArgs(2));

    // Copy all input values. The input data is interpreted as a byte array:
    PsychCopyInIntegerArg(1, TRUE, &usbHandle);
    PsychCopyInIntegerArg(2, TRUE, &endPoint);
    PsychCopyInIntegerArg(3, TRUE, &length);

    // Get 'dev'icerecord for handle: This will error-out if no such device open:
    dev = PsychHIDGetUSBDevice(usbHandle);

    // IN-Transfer for reception of data from USB device?
    if (endPoint & USB_INTRANSFER) {
        if (length <= 0) {
            PsychErrorExitMsg(PsychError_user, "Argument length must be > 0 for an in-transfer command!");
        }

        // Allocate return buffer of sufficient size wLength:
        m = 1; n = length; p = 1;
        PsychAllocOutUnsignedByteMatArg(2, TRUE, m, n, p, &buffer);
    }
    else {
        // Get the outData buffer if it was specified.
        m = n = p = 0;
        PsychAllocInUnsignedByteMatArg(4, FALSE, &m, &n, &p, &buffer);
        if (((m * n) > 0) && (p != 1))
            PsychErrorExitMsg(PsychError_user, "Argument outData must be a 1D vector or 2D matrix of bytes! This is a 3D matrix!");

        // Is the outData buffer at least as big as the provided length argument?
        if ((m * n) < length)
            PsychErrorExitMsg(PsychError_user, "Argument outData is missing or has less elements than provided length argument! This is too little!");
    }

    // Timeout provided? Get it and validate:
    if (PsychCopyInIntegerArg(5, FALSE, &timeOutMSecs) && (timeOutMSecs < 0))
        PsychErrorExitMsg(PsychError_user, "Argument timeOutMSecs is negative, but must be at least 0 milliseconds!");

    // Make the actual bulk transfer request:
    if ((err = PsychHIDOSBulkTransfer(dev, (psych_uint8) endPoint, length, buffer, &count, (unsigned int) timeOutMSecs)) < 0)
        PsychErrorExitMsg(PsychError_system, "The USB Bulk transfer failed.");

    // Return the number of actually transfered bytes:
    PsychCopyOutDoubleArg(1, FALSE, (double) count);

    return(PsychError_none);
}

PsychError PSYCHHIDUSBInterruptTransfer(void)
{
    static char useString[] = "[count, recData] = PsychHID('USBInterruptTransfer', usbHandle, endPoint, length [, outData][, timeOutMSecs=10000])";
    //                          1      2                                           1          2         3         4          5
    static char synopsisString[] =  "Performs a USB interrupt transfer.\n"
    "The results of in-transfers are returned in the 2nd return argument 'recData' as a uint8 array "
    "of length 'length'. The amount of actually received data can be less than 'length' and is "
    "returned in the return argument 'count'. Therefore, in case of short in-transfers, 'count' "
    "can be less than 'length' (which is also the length of 'recData' (iow. length(recData)).\n"
    "In case of an out-transfer, 'recData' does not exist, and 'count' will instead be the "
    "only return argument, reporting the amount of actual data sent to the device. 'outData' must be "
    "a uint8() array of at least 'length' bytes for out-transfers, but all bytes beyond the first "
    "'length' bytes will be ignored.\n"
    "The actual number of bytes transfered in any direction is returned in return argument 'count'.\n"
    "'usbHandle' is the handle of the USB device to perform the transfer on.\n"
    "'endPoint' is the end-point address: If bit 7 is set, this defines a interrupt in-transfer from "
    "device to host and 'recData' will be filled with at most 'length' bytes received from the device. "
    "Otherwise it defines a interrupt out-transfer from host to device and at most 'length' bytes will be "
    "transferred from 'outData' to the device.\n"
    "'length' is the amount of data to return at most on an in-transfer, or the amount of data to send at "
    "most for an out-transfer in the optional uint8 vector 'outData'. 'outData' must have at least as many "
    "elements as the value of 'length'!\n"
    "'timeOutMSecs' is an optional timeout for the operation, in milliseconds. Default is 10000 msecs. "
    "A value of zero means to never time out, but wait indefinitely.\n";
    static char seeAlsoString[] =   "OpenUSBDevice USBControlTransfer USBBulkTransfer";

    PsychUSBDeviceRecord *dev;
    int usbHandle, endPoint, length, count;
    int m, n, p, err;
    psych_uint8 *buffer = NULL;
    int timeOutMSecs = 10000;

    // Setup the help features.
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return PsychError_none; }

    // Make sure the correct number of input arguments is supplied:
    PsychErrorExit(PsychRequireNumInputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(5));
    PsychErrorExit(PsychCapNumOutputArgs(2));

    // Copy all input values. The input data is interpreted as a byte array:
    PsychCopyInIntegerArg(1, TRUE, &usbHandle);
    PsychCopyInIntegerArg(2, TRUE, &endPoint);
    PsychCopyInIntegerArg(3, TRUE, &length);

    // Get 'dev'icerecord for handle: This will error-out if no such device open:
    dev = PsychHIDGetUSBDevice(usbHandle);

    // IN-Transfer for reception of data from USB device?
    if (endPoint & USB_INTRANSFER) {
        if (length <= 0) {
            PsychErrorExitMsg(PsychError_user, "Argument length must be > 0 for an in-transfer command!");
        }

        // Allocate return buffer of sufficient size wLength:
        m = 1; n = length; p = 1;
        PsychAllocOutUnsignedByteMatArg(2, TRUE, m, n, p, &buffer);
    }
    else {
        // Get the outData buffer if it was specified.
        m = n = p = 0;
        PsychAllocInUnsignedByteMatArg(4, FALSE, &m, &n, &p, &buffer);
        if (((m * n) > 0) && (p != 1))
            PsychErrorExitMsg(PsychError_user, "Argument outData must be a 1D vector or 2D matrix of bytes! This is a 3D matrix!");

        // Is the outData buffer at least as big as the provided length argument?
        if ((m * n) < length)
            PsychErrorExitMsg(PsychError_user, "Argument outData is missing or has less elements than provided length argument! This is too little!");
    }

    // Timeout provided? Get it and validate:
    if (PsychCopyInIntegerArg(5, FALSE, &timeOutMSecs) && (timeOutMSecs < 0))
        PsychErrorExitMsg(PsychError_user, "Argument timeOutMSecs is negative, but must be at least 0 milliseconds!");

    // Make the actual interrupt transfer request:
    if ((err = PsychHIDOSInterruptTransfer(dev, (psych_uint8) endPoint, length, buffer, &count, (unsigned int) timeOutMSecs)) < 0)
        PsychErrorExitMsg(PsychError_system, "The USB Bulk transfer failed.");

    // Return the number of actually transfered bytes:
    PsychCopyOutDoubleArg(1, FALSE, (double) count);

    return(PsychError_none);
}
