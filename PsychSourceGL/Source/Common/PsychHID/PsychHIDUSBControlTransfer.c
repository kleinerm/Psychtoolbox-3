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
                                "The results of in-transfers are returned in the 1st return argument 'recData' as a uint8() row vector "
                                "of length 'wLength'. The amount of actually received data can be less than 'wLength' and is "
                                "returned in the 2nd return argument 'count'. Therefore, in case of shortened in-transfers, 'count' "
                                "can be less than 'wLength' (which is also the length of 'recData' (iow. length(recData)).\n"
                                "In case of an out-transfer, 'recData' does not exist, and 'count' will instead be the 1st and "
                                "only return argument, reporting the amount of actual data sent to the device. 'outData' must be "
                                "a uint8() vector of at least 'wLength' bytes for out-transfers, but all bytes beyond the first "
                                "'wLength' bytes will be ignored.\n"
                                "The actual number of bytes transfered in any direction is returned in return argument 'count'.\n"
                                "Please note that for control transfers where an endpoint (2) or interface (1) is the recipient, ie. where "
                                "ismember(bitand(bmRequestType, 0x1f), [1, 2]), the interface (or associated interface of the endpoint) "
                                "must be explicitely claimed via PsychHID('USBClaimInterface', usbHandle, interface); before calling the "
                                "control transfer. Interface number (1) or endpoint address (2) are specified in 'wIndex'. The only exception "
                                "from this rule is endpoint 0, or control transfers to other targets, e.g., the device itself.\n"
                                "'usbHandle' is the handle of the USB device to control.\n"
                                "'bmRequestType' is the type of request, an 8 bit bit-mask: If bit 7 is set, this defines an in-transfer "
                                "from device to host and 'recData' will be filled with at most 'wLength' bytes received from the device. "
                                "Otherwise it defines a transfer from host to device and at most 'wLength' bytes will be transferred from "
                                "'outData' to the device.\n"
                                "'bRequest' is the 8 bit range request id.\n"
                                "'wValue' and 'wIndex' are device- and request specific values in the 16 bit range 0 - 65535.\n"
                                "'wLength' is the amount of data to return at most on an in-transfer, or the amount of data to send at "
                                "most for an out-transfer in the optional uint8 vector 'outData'. 'outData' must have at least as many "
                                "elements as the value of 'wLength'!\n"
                                "'timeOutMSecs' is an optional timeout for the operation, in milliseconds. Default is 10000 msecs. "
                                "A value of zero means to never time out, but wait indefinitely.\n";
static char seeAlsoString[] =   "OpenUSBDevice USBClaimInterface USBBulkTransfer USBInterruptTransfer";

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
    static char useString[] = "[countOrRecData] = PsychHID('USBBulkTransfer', usbHandle, endPoint, length [, outData][, timeOutMSecs=10000])";
    //                          1                                             1          2         3         4          5
    static char synopsisString[] =  "Performs a synchronous USB bulk transfer.\n"
                                    "The function will automatically claim interface #0 to enable this transfer, unless "
                                    "you call PsychHID('USBClaimInterface', usbHandle, interfaceId) first to claim a "
                                    "different interface 'interfaceId' for accessing the wanted endPoint.\n"
                                    "The results of in-transfers are returned in 'countOrRecData' as an uint8() row vector. "
                                    "The amount of actually received data can be less than the requested 'length'. "
                                    "In case of an out-transfer, 'countOrRecData' reports the amount of actual data sent.\n"
                                    "'usbHandle' is the handle of the USB device to perform the transfer to or from.\n"
                                    "'endPoint' is the USB end-point address: If its bit 7 is set, this requests an in-transfer from "
                                    "device to host and 'countOrRecData' will contain at most 'length' bytes received from the device. "
                                    "Otherwise it requests an out-transfer from host to device and 'outData' will be sent to the device.\n"
                                    "'length' is the amount of data to return on an in-transfer. It is ignored for out-transfers.\n"
                                    "'outData' is a uint8() data vector to send. It will be ignored for in-transfers.\n"
                                    "'timeOutMSecs' is an optional timeout for the operation, in milliseconds. Default is 10000 msecs. "
                                    "A value of zero means to never time out, but wait indefinitely for transfer completion.\n";
    static char seeAlsoString[] =   "OpenUSBDevice USBClaimInterface USBControlTransfer USBInterruptTransfer";

    PsychUSBDeviceRecord *dev;
    int usbHandle, endPoint, length, count;
    int m, n, p, err;
    psych_uint8 *buffer = NULL;
    psych_uint8 *outBuffer = NULL;
    int timeOutMSecs = 10000;

    // Setup the help features.
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return PsychError_none; }

    // Make sure the correct number of input arguments is supplied:
    PsychErrorExit(PsychRequireNumInputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(5));
    PsychErrorExit(PsychCapNumOutputArgs(1));

    // Copy all input values. The input data is interpreted as a byte array:
    PsychCopyInIntegerArg(1, TRUE, &usbHandle);
    PsychCopyInIntegerArg(2, TRUE, &endPoint);

    // Get 'dev'icerecord for handle: This will error-out if no such device open:
    dev = PsychHIDGetUSBDevice(usbHandle);

    // IN-Transfer for reception of data from USB device?
    if (endPoint & USB_INTRANSFER) {
        // Get the mandatory length of data to receive:
        PsychCopyInIntegerArg(3, TRUE, &length);
        if (length <= 0) {
            PsychErrorExitMsg(PsychError_user, "Argument length must be > 0 for an in-transfer command!");
        }

        // Allocate temporary receive buffer for max expected amount of length bytes,
        // plus 1 byte to compensate for bug in libusb 1.0.26 windows hid backend:
        buffer = PsychMallocTemp(length + 1);
    }
    else {
        // Get the mandatory outData buffer with data to send:
        PsychAllocInUnsignedByteMatArg(4, TRUE, &m, &n, &p, &buffer);
        length = m * n;
        if ((length <= 0) || (p != 1))
            PsychErrorExitMsg(PsychError_user, "Argument outData must be a 1D vector or 2D matrix of at least 1 bytes size! This is a 3D matrix, or it is empty!");
    }

    // Timeout provided? Get it and validate:
    if (PsychCopyInIntegerArg(5, FALSE, &timeOutMSecs) && (timeOutMSecs < 0))
        PsychErrorExitMsg(PsychError_user, "Argument timeOutMSecs is negative, but must be at least 0 milliseconds!");

    // Make the actual bulk transfer request:
    if ((err = PsychHIDOSBulkTransfer(dev, (psych_uint8) endPoint, length, buffer, &count, (unsigned int) timeOutMSecs)) < 0)
        PsychErrorExitMsg(PsychError_system, "The USB Bulk transfer failed.");

    // Return arg 1 is either the received data buffer, or number of sent out bytes:
    if (endPoint & USB_INTRANSFER) {
        // Workaround for off-by-one bug in libusb <= v1.0.26 MS-Windows/hid backend,
        // as that one can return a buffer with one byte too much, a 0 postfix byte.
        if (count > length)
            count = length;

        // Allocate return buffer of sufficient size for count received bytes:
        PsychAllocOutUnsignedByteMatArg(1, FALSE, 1, count, 1, &outBuffer);

        // Copy received data from temp buffer to output buffer:
        memcpy(outBuffer, buffer, count);
    }
    else {
        // Return the number of actually transfered bytes during out transfer:
        PsychCopyOutDoubleArg(1, FALSE, (double) count);
    }

    return(PsychError_none);
}

PsychError PSYCHHIDUSBInterruptTransfer(void)
{
    static char useString[] = "[countOrRecData] = PsychHID('USBInterruptTransfer', usbHandle, endPoint, length [, outData][, timeOutMSecs=10000])";
    //                          1                                                  1          2         3         4          5
    static char synopsisString[] =  "Performs a synchronous USB interrupt transfer.\n"
                                    "The function will automatically claim interface #0 to enable this transfer, unless "
                                    "you call PsychHID('USBClaimInterface', usbHandle, interfaceId) first to claim a "
                                    "different interface 'interfaceId' for accessing the wanted endPoint.\n"
                                    "The results of in-transfers are returned in 'countOrRecData' as an uint8() row vector. "
                                    "The amount of actually received data can be less than the requested 'length'. "
                                    "In case of an out-transfer, 'countOrRecData' reports the amount of actual data sent.\n"
                                    "'usbHandle' is the handle of the USB device to perform the transfer to or from.\n"
                                    "'endPoint' is the USB end-point address: If its bit 7 is set, this requests an in-transfer from "
                                    "device to host and 'countOrRecData' will contain at most 'length' bytes received from the device. "
                                    "Otherwise it requests an out-transfer from host to device and 'outData' will be sent to the device.\n"
                                    "'length' is the amount of data to return on an in-transfer. It is ignored for out-transfers.\n"
                                    "'outData' is a uint8() data vector to send. It will be ignored for in-transfers.\n"
                                    "'timeOutMSecs' is an optional timeout for the operation, in milliseconds. Default is 10000 msecs. "
                                    "A value of zero means to never time out, but wait indefinitely for transfer completion.\n";
    static char seeAlsoString[] =   "OpenUSBDevice USBClaimInterface USBControlTransfer USBBulkTransfer";

    PsychUSBDeviceRecord *dev;
    int usbHandle, endPoint, length, count;
    int m, n, p, err;
    psych_uint8 *buffer = NULL;
    psych_uint8 *outBuffer = NULL;
    int timeOutMSecs = 10000;

    // Setup the help features.
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return PsychError_none; }

    // Make sure the correct number of input arguments is supplied:
    PsychErrorExit(PsychRequireNumInputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(5));
    PsychErrorExit(PsychCapNumOutputArgs(1));

    // Copy all input values. The input data is interpreted as a byte array:
    PsychCopyInIntegerArg(1, TRUE, &usbHandle);
    PsychCopyInIntegerArg(2, TRUE, &endPoint);

    // Get 'dev'icerecord for handle: This will error-out if no such device open:
    dev = PsychHIDGetUSBDevice(usbHandle);

    // IN-Transfer for reception of data from USB device?
    if (endPoint & USB_INTRANSFER) {
        // Get the mandatory length of data to receive:
        PsychCopyInIntegerArg(3, TRUE, &length);
        if (length <= 0) {
            PsychErrorExitMsg(PsychError_user, "Argument length must be > 0 for an in-transfer command!");
        }

        // Allocate temporary receive buffer for max expected amount of length bytes,
        // plus 1 byte to compensate for bug in libusb 1.0.26 windows hid backend:
        buffer = PsychMallocTemp(length + 1);
    }
    else {
        // Get the mandatory outData buffer with data to send:
        PsychAllocInUnsignedByteMatArg(4, TRUE, &m, &n, &p, &buffer);
        length = m * n;
        if ((length <= 0) || (p != 1))
            PsychErrorExitMsg(PsychError_user, "Argument outData must be a 1D vector or 2D matrix of at least 1 bytes size! This is a 3D matrix, or it is empty!");
    }

    // Timeout provided? Get it and validate:
    if (PsychCopyInIntegerArg(5, FALSE, &timeOutMSecs) && (timeOutMSecs < 0))
        PsychErrorExitMsg(PsychError_user, "Argument timeOutMSecs is negative, but must be at least 0 milliseconds!");

    // Make the actual interrupt transfer request:
    if ((err = PsychHIDOSInterruptTransfer(dev, (psych_uint8) endPoint, length, buffer, &count, (unsigned int) timeOutMSecs)) < 0)
        PsychErrorExitMsg(PsychError_system, "The USB Interrupt transfer failed.");

    // Return arg 1 is either the received data buffer, or number of sent out bytes:
    if (endPoint & USB_INTRANSFER) {
        // Workaround for off-by-one bug in libusb <= v1.0.26 MS-Windows/hid backend,
        // as that one can return a buffer with one byte too much, a 0 postfix byte.
        if (count > length)
            count = length;

        // Allocate return buffer of sufficient size for count received bytes:
        PsychAllocOutUnsignedByteMatArg(1, FALSE, 1, count, 1, &outBuffer);

        // Copy received data from temp buffer to output buffer:
        memcpy(outBuffer, buffer, count);
    }
    else {
        // Return the number of actually transfered bytes during out transfer:
        PsychCopyOutDoubleArg(1, FALSE, (double) count);
    }

    return(PsychError_none);
}

PsychError PSYCHHIDUSBClaimInterface(void)
{
    static char useString[] = "PsychHID('USBClaimInterface', usbHandle, interfaceId)";
    //                                                       1          2
    static char synopsisString[] =  "Claim a USB interface.\n"
                                    "The function will try to detach kernel drivers potentially attached to "
                                    "the interface already. If a data transfer operation on an endpoint is "
                                    "requested before a specific interface has been claimed by calling this "
                                    "function, e.g., an USBBulkTransfer or USBInterruptTransfer is attempted, "
                                    "then interface number 0 will automatically first be claimed by default.\n\n"
                                    "'usbHandle' is the handle of the USB device to claim an interface from.\n"
                                    "'interfaceId' is the bInterfaceNumber of the interface to claim.\n";
    static char seeAlsoString[] =   "OpenUSBDevice USBControlTransfer USBBulkTransfer USBInterruptTransfer";

    PsychUSBDeviceRecord *dev;
    int usbHandle, interfaceId;

    // Setup the help features.
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return PsychError_none; }

    // Make sure the correct number of input arguments is supplied:
    PsychErrorExit(PsychRequireNumInputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychCapNumOutputArgs(0));

    PsychCopyInIntegerArg(1, TRUE, &usbHandle);
    PsychCopyInIntegerArg(2, TRUE, &interfaceId);

    // Get 'dev'icerecord for handle: This will error-out if no such device open:
    dev = PsychHIDGetUSBDevice(usbHandle);

    // Try to claim interface:
    if (PsychHIDOSClaimInterface(dev, interfaceId) < 0)
        PsychErrorExitMsg(PsychError_system, "Claiming the USB interface failed.");

    return(PsychError_none);
}

