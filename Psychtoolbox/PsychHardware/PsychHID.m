function varargout = PsychHID(varargin)  %#ok<STOUT>
% 
% returnValues=PsychHID(subcommand, arg1 [,arg2] ...)
%  
% PsychHID is a MEX file that communicates with any USB device that is
% HID-compliant. The Universal Serial Bus (USB) is quite popular, and there
% are many USB devices that conform to the Human Interface Device (HID)
% class. Typical HID devices are keyboards, mice and joysticks. That is
% what the HID class was designed for, but manufacturers of more exotic
% input and output devices, including audio breakout boxes and data
% acquisition devices, have also opted to be HID-compliant.
%
% PsychHID also supports certain low-level operations on USB devices which are not
% HID-compliant. Currently synchronous USB control transfers, interrupt transfers,
% and bulk transfers are supported. PsychHID uses libusb-1.0 for these operations.
% Abilities and limitations of low-level access, and various required setup steps
% to enable low-level access to specific USB devices, vary by operating system,
% available operating system or 3rd party (device manufacturer) device drivers,
% and the specific type and capabilities of USB devices. In general, things should
% work nicely on Linux if a suitable udev.rules file is installed for your device.
% Things should work on macOS if neither macOS itself nor 3rd parties (e.g., the
% device manufacturer of your USB device) have a kernel driver attached to your
% device for exclusive use. In case of attached kernel drivers, only various painful
% measures and hacks will make it maybe possible to access the device. See the
% libusb FAQ here:
%
% https://github.com/libusb/libusb/wiki/FAQ#how-can-i-run-libusb-applications-under-mac-os-x-if-there-is-already-a-kernel-extension-installed-for-the-device-and-claim-exclusive-access
%
% On MS-Windows, abilities depend on specific type of device. Some will work plug
% and play out of the box without limitations, others will have limitations and/or
% require installation of dedicated kernel drivers. E.g., USB-HID devices which are
% accessed by the libusb HID backend can not support USB control transfers as of
% libusb version 1.0.26. See these FAQ's for setup info:
%
% https://github.com/libusb/libusb/wiki/FAQ#How_to_use_libusb_under_Windows
%
% 
% NOT RESPONDING? If PsychHID is not responding, try quitting and
% restarting MATLAB. We find that this reliably restores normal
% communication. 
%
%
% Additional software requirements:
%
% PsychHID uses the free/open-source libusb-1.0 library (http://libusb.info)
% as a backend for low-level device control. libusb is licensed under
% LGPLv2+ license. PsychHID also uses the BSD licensed HIDAPI library
% (http://www.signal11.us/oss/hidapi/) as backend for USB-HID access.
%
% On Linux and Windows, not all PsychHID subfunctions are implemented, as
% certain functionality can be handled in a better or different way on
% those systems. USB-HID low-level access, and USB low-level access is
% implemented though.
%
% Linux: libusb-1 is part of a standard operating system installation, so
% nothing to do for you.
%
% macOS: You must manually install the libusb-1.0.dylib library on your
% system to use PsychHID for low-level USB access and communication, e.g.,
% for synchronous USB control transfers, bulk- and interrupt transfers.
%
% One good source of libusb-1.0.dylib is the HomeBrew package manager from
% https://brew.sh After setting up HomeBrew you can install libusb-1 via:
%
% brew install libusb
%
% MS-Windows: PsychHID will use a working version of libusb-1.0.dll which is
% contained in the Psychtoolbox/PsychContributed/x64 folder. This library is
% only infrequently updated by us. More recent versions of the library may be
% downloaded from the official project website:
% https://libusb.info -> Download link, or Git repository releases section...
% https://github.com/libusb/libusb/releases
%
%
% HELP: Like Screen, PsychHID has built-in help. For a list of PsychHID
% subcommands enter "PsychHID" at the MATLAB command line:
% 
% >> PsychHID
% Usage:
% numberOfDevices=PsychHID('NumDevices')
% numberOfElements=PsychHID('NumElements',deviceNumber)
% numberOfCollections=PsychHID('NumCollections',deviceNumber)
% devices=PsychHID('Devices')
% elements=PsychHID('Elements',deviceNumber)
% collections=PsychHID('Collections',deviceNumber)
% elementState=PsychHID('RawState',deviceNumber,elementNumber)
% elementState=PsychHID('CalibratedState',deviceNumber,elementNumber)
% [keyIsDown,secs,keyCode]=PsychHID('KbCheck',[deviceNumber])
% [report,err]=PsychHID('GetReport',deviceNumber,reportType,reportID,reportBytes)
% err=PsychHID('SetReport',deviceNumber,reportType,reportID,report)
% usbHandle = PsychHID('OpenUSBDevice', vendorID, deviceID [, configurationId=0])
% PsychHID('CloseUSBDevice' [, usbHandle])
% PsychHID('USBClaimInterface', usbHandle, interfaceId)
% [recData, count] = PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength [, outData][, timeOutMSecs=10000])
% [countOrRecData] = PsychHID('USBBulkTransfer', usbHandle, endPoint, length [, outData][, timeOutMSecs=10000])
% [countOrRecData] = PsychHID('USBInterruptTransfer', usbHandle, endPoint, length [, outData][, timeOutMSecs=10000])
%
% For help on a specific PsychHID subcommand, call PsychHID with the
% subcommand suffixed with a question mark, for example:
%
%   >> PsychHID NumDevices?
%   Usage:
%   numberOfDevices=PsychHID('NumDevices')
%   Return the the number of USB HID devices connected to your computer.
%
% History:
%
% In the OS9 and Win Psychtoolbox, most functions that accepted user input
% were implemented as device-specific mex files. In Psychtoolbox-3 they are
% M files that all call PsychHID, just one MEX for the many kinds of input
% device.  For example, Gamepad (formerly named "Joystick") and KbCheck are
% M functions that call PsychHID.  For user programs, it is easier to call
% these special-purpose functions (with self-documenting names), which call
% PsychHID, than to call PsychHID directly. PsychHID is more general, but
% also more complicated. However, if you acquire a novel input/output
% device that is HID-compliant, you can access it through PsychHID.
% PsychHID can distinguish between multiple devices of the same type by
% their serialNumber or locationID.
%
% More technical background infos:
%
% PsychHID on macOS links against Apples HID Utilities library.  For information on
% how to program HID devices on OS X in C or Objective C see the HID
% utilities project, its companion project, HID Explorer and related:
% http://developer.apple.com/samplecode/HID_Utilities_Source/HID_Utilities_Source.html
% http://developer.apple.com/samplecode/HID_Explorer/HID_Explorer.html
% http://developer.apple.com/samplecode/Hardware/idxHumanInterfaceDeviceForceFeedback-date.html
% _________________________________________________________________________
%  
% See also: DaqTest, KbCheck, Gamepad, PsychHardware. 

AssertMex('PsychHID.m');
