/*
 *  IOWarriorLib.h
 *  IOWarriorHIDTest
 *
 *  Created by ilja on Sun Dec 29 2002.
 *  $Id: IOWarriorLib.h,v 1.3 2004/02/25 21:25:02 ilja Exp $
 *
 */

/*!
@header IOWarriorLib
 The IOWarrior Library provides convinience funtions to access the IOWarrior under Mac OS X 10.2.3 or later. It simplifies the process of finding and accessing IOWarrior devices .

 You will need to link your software against the IOKit.framework in order to use this source code.

 Make sure you are calling IOWarriorInit before calling any of the other functions.
 
 If you are running Mac OS X 10.2.3, make sure you have updated your Developer Tools at least to the December 2002 version. 
 
 Binaries compiled on Mac OS X 10.3 using this version of the IOWarrior Library will not work on earlier systems. However, binaries on 10.2.3 should also work on 10.2.3 or later. If you are using 10.3 and want to develop appplicaton that run on 10.2.x, too, select the 10.2.7 Cross Development SDK in XCode targets inspector. You have to install the additional SDK when installing the Mac OS X Developer Tools.

 $Id: IOWarriorLib.h,v 1.3 2004/02/25 21:25:02 ilja Exp $

 */

#include <Carbon/Carbon.h>
#include <IOKit/hid/IOHIDLib.h>

#define kIOWarriorVendorID 1984
#define kIOWarrior40DeviceID 0x1500
#define kIOWarrior24DeviceID 0x1501

#ifdef kIOHIDDeviceInterfaceID122
// we are running on 10.3 or later
#define IOWarriorHIDDeviceInterface IOHIDDeviceInterface122
#define kIOWarriorHIDDeviceInterfaceID kIOHIDDeviceInterfaceID122
#else
// running on 10.2.x
#define IOWarriorHIDDeviceInterface IOHIDDeviceInterface
#define kIOWarriorHIDDeviceInterfaceID kIOHIDDeviceInterfaceID
#endif


/*!
@defined kIOWarrior40Interface0
@discussion Interface type indentifier for an interface 0 of an IOWarrior 40.
*/
#define kIOWarrior40Interface0 0

/*!
@defined kIOWarrior40Interface1
 @discussion Interface type indentifier for an interface 1 of an IOWarrior 40.
 */
#define kIOWarrior40Interface1 1

/*!
@defined kIOWarrior24Interface0
 @discussion Interface type indentifier for an interface 0 of an IOWarrior 24.
 */
#define kIOWarrior24Interface0 2

/*!
@defined kIOWarrior24Interface1
 @discussion Interface type indentifier for an interface 0 of an IOWarrior 24.
 */
#define kIOWarrior24Interface1 3

/*!
@struct IOWarriorListNodeStruct
 @discussion A structure implementing a linked list node. Used to keep track of all IOWarrior Interfaces
 connected to the system. You can use IOWarriorCountInterfaces and IOWarriorInterfaceListNodeAtIndex to iterate trough all interfaces.
 
 @field ioWarriorHIDInterface 	Interface object for the systems IOKit
 @field nextNode 		Pointer to the node in the interface list.
 @field serialNumber		The serial number of the interface.
 @field interfaceType		The type of the interface (kIOWarrior40Interface0, kIOWarrior40Interface1, ..) . 
 */

IONotificationPortRef GetNotificationPort ();


struct IOWarriorListNodeStruct
{
    IOWarriorHIDDeviceInterface**   ioWarriorHIDInterface;			// IOKits interface object
    struct                          IOWarriorListNodeStruct* nextNode;	// pointer to the next interface
    CFStringRef						serialNumber;				// the device serial number
    int                             interfaceType;				// the type of the interface
};

typedef struct IOWarriorListNodeStruct IOWarriorListNode;


/*!
@typedef IOWarriorCallbackFunctionPtr
 @abstract A function pointer type for IOWarriors callback function.
 @discussion A function pointer type for IOWarriors callback function, that gets called whenever an IOWarrior device is added or removed. You can install you own callback function using the IOWarriorSetCallback fucntion.
 */
typedef void (*IOWarriorDeviceCallbackFunctionPtr) (void* inRefCon);
 
/*!
@function IOWarriorInit
@abstract Initializes the IOWarrior library. Call this before calling any other functions.
@result Returns 0 if successfull, otherwise return value is different from 0.
 */
int IOWarriorInit ();

/*!
@function IOWarriorIsPresent
@abstract Use this function to too see of an IOWarrior device is present.
@result Returns 1 if at least one IOWarrior is connected to this system. Returns 0 if no IOWarrior device could be discovered.
 */
int IOWarriorIsPresent ();

/*!
@function IOWarriorCount
 @abstract Returns the number of interfaces connected to this system.
 @result The number of IOWarrior interfaces. 
 */
int IOWarriorCountInterfaces ();

/*!
@function IOWarriorWriteInterface0
@abstract Writes a 4 byte buffer (32 bit) to interface 0 of the first IOWarrior 40 devices using a SetOutputReport request.
@discussion The first byte of the passed buffer becomes mapped to the pins of port 0, the second byte to the pins of port 1 and so on.
@param inData A pointer to the data being written. Should be at least 4 bytes long.
@result Returns 0 if writing operation was successfull. Returns a value different from 0 otherwise.
 */
int IOWarriorWriteInterface0 (void *inData);

/*!
@function IOWarriorReadInterface0
@abstract Reads 4 byte (32 bit) from interface 0 of the first IOWarrior 40 device connected to this system using a GetOutputReport request.
 @param outData A pointer to a buffer where the data should be stored. Buffer has to be at least 4 bytes long
 @result Returns 0 if reading operation was successfull. Returns a value different from 0 otherwise.
*/
int IOWarriorReadInterface0 (void *outData);

/*!
@function IOWarriorWriteInterface1
 @abstract Writes a 7 byte output report to interface 1 of the first IOWarrior 40 device connected to this system.
 @discussion Use the function to control IOWarrior special modes (like IIC and LCD mode) of the IOWarrior. See the IOWarrior datasheet for details.
 @param inReportID The report ID of the request. See the IO Warrior Datasheet for the appropriate values.
 @param inData A pointer to the data being written. Should be at least 7 bytes long.
 @result Returns 0 if writing operation was successfull. Returns a value different from 0 otherwise.
 */
int IOWarriorWriteInterface1 (int inReportID, void *inData);

/*!
@function IOWarriorReadInterface1
 @abstract Reads a 7 byte output report from interface 1 of the first IOWarrior 40 device connected to this system.
  This function won't work for interfaces 1 of an IOWarrior 40 on Mac OS X 10.2.x.
 
 @discussion Use this function to obtain data from the IOWarrior when in special mode.  See the IOWarrior datasheet for details.
 @param inReportID The report ID of the request. See the IO Warrior Datasheet for the appropriate values.
 @param outData A pointer to a buffer where the data should be stored. Buffer has to be at least 7 bytes long
 @result Returns 0 if reading operation was successfull. Returns a value different from 0 otherwise.
 */
int IOWarriorReadInterface1 (int inReportID, void *outData);

/*!
@function IOWarriorFirstInterfaceOfType
 @abstract Returns the first IOWarrior interface of type inInterfaceType. 
 
 @discussion Use this function to get the first discovered interface of a certain type. See the Constants definitions for a list of supported IOWarrior interface types. The result of this function can be used as paramter for IOWarriorWriteToInterface or IOWarriorReadFromInterface. Returns NULL if no interface matching inInterfaceType could be found.
 @param inInterfaceType The type of the interface to be returned.
 @result Returns an interface object if found, NULL otherwise.
 */
IOWarriorHIDDeviceInterface** IOWarriorFirstInterfaceOfType (int inInterfaceType);


/*!
@function IOWarriorInterfaceListNodeAtIndex
 @abstract Returns the list node element at index inIndex.
 @discussion Use this function to obtain more information (serial number, device type) about an interface of an IOWarrior device.
 @param inIndex The index of the desired list node. First list node has index 0.
 @result A pointer to a structure of type IOWarriorListNode.  NULL if index is out of bounds.
 */
IOWarriorListNode* IOWarriorInterfaceListNodeAtIndex (int inIndex);

/*!
@function IOWarriorWriteToInterface
 @abstract Writes data to a specific interface.
 @discussion Use this function to write to a certain amount of data associated with a certain reportID to a specific interface.
 
 @param inInterface The interface the data should be written to. Pass the ioWarriorHIDInterface member of an IOWarriorListNode struct obtained earlier.
 @param inSize The size of the data to be written.
 @param inData A pointer to the data to be written. First byte is report id.
 @result Returns 0 if write operation was successful, returns an error code different from 0 otherwise.
 */
int IOWarriorWriteToInterface (IOWarriorHIDDeviceInterface** inInterface, int inSize, void* inData);

/*!
@function IOWarriorReadFromInterface
 @abstract Reads data from a specific interface.
 @discussion Use this function to read a certain amount of data associated with a certain reportID to a specific interface.

 @param inInterface The interface the data should be read from. Pass the ioWarriorHIDInterface member of an IOWarriorListNode struct.  This function will not get you the data sent in interrupt mode from interface 1. Instead you will receive a copy of the last HID report sent in interrupt mode. Use IOWarriorSetInterruptCallback to receive data received in interrupt mode.
 @param inReportID The report ID of the request. See the IO Warrior Datasheet for the appropriate values.
 @param inSize The size of the data to be read.
 @param inData A pointer to a buffer thats at least inSize bytes long. Read data will be stored here.
 @result Returns 0 if read operation was successful, returns an error code different from 0 otherwise.
 */
int IOWarriorReadFromInterface (IOWarriorHIDDeviceInterface** inInterface, int inReportID, int inSize, void* outData);

/*!
@function IOWarriorSetDeviceCallback
 @abstract Sets the function to be called when IOWarrior devices ar added or removed. 
 @discussion Use this function to install a custom callback function. You function will be called whenever IOWarrior devices are added or removed from the system. You should invalidate any references to IOWarriorListNode structures you might have saved when your callback functions gets called. Be sure to call IOWarriorCountInterfaces at least once after you callback function was invoked before calling any other functions from the IOWarrior Library.
 */
void IOWarriorSetDeviceCallback (IOWarriorDeviceCallbackFunctionPtr inCallbackPtr, void* inRefCon);

/*!
@function IOWarriorSetInterruptCallback
 @abstract Sets the function to be called when a report data is received on inInterface. 
 @discussion Use this function to install a custom callback routine that gets invoked when data is received on interface inInterface. This function will only work when your application was compiled on Mac OS 10.3 or later and is running on Mac OS 10.3 or later. For earlier systems the implementation of the function is hidden from the compiler using preprocessor commands, because the system does not implement the required API calls.
 @param inInterface The interface where your data arrives
 @param inBuffer A buffer provided by the caller.
 @param inBufferSize The size of inBuffer.
 @param inCallbackPtr A pointer to your callback function. See IOHIDLib.h for more info.
 @param inRefCon A reference value passed to your callback on invocation.
 */
int IOWarriorSetInterruptCallback (IOWarriorHIDDeviceInterface** inInterface, void* inBuffer, UInt32 inBufferSize, 
                                  IOHIDReportCallbackFunction inCallbackPtr, void* inRefCon);

