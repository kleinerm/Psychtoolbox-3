/*
 *  ColorCal.h
 *  ColorCal2
 *
 *  Created by Christopher Broussard on 2/22/09.
 *
 */

#ifndef __COLORCAL_H__
#define __COLORCAL_H__

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <mach/mach.h>
#include <string.h>
#include <mex.h>

#define kColorCal2VendorID        0x0861    // Vendor ID of the ColorCal2
#define kColorCal2ProductID       0x1001    // Product ID of the ColorCal2

// Function Declarations
IOReturn ConfigureDevice(IOUSBDeviceInterface **dev);
bool OpenDevice(void);
static void CloseDevice(void);
double DefunnyMatrixValue(int value);
UInt16 RefunnyMatrixValue(double value);

#endif
