function PsychHID 
% 
% returnValues=PsychHID(subcommand, arg1 [,arg2] ...)
%  
% OSX: ___________________________________________________________________
%  
% PsychHID is a Matlab MEX file that communicates with any USB device that
% is HID-compliant. The Universal Serial Bus (USB) is quite popular, and
% there are many USB devices that conform to the Human Interface Device
% (HID) class.  Typical HID devices are keyboards, mice and joysticks. That
% is what the HID class was designed for, but manufacturers of more exotic
% input and output devices, including audio breakout boxes and data
% acquisition devices, have also opted to be HID-compliant.
% 
% In the OS9 and Win Psychtoolbox, most functions that accept user input
% were implemented as device-specific mex files. In the OSX Psychtoolbox
% they are M files that all call PsychHID, just one MEX for the many kinds
% of input device.  For example, in the OSX Psychtoolbox, Gamepad (formerly
% named "Joystick") and KbCheck are M functions that call PsychHID.  For
% user programs, it is easier to call these special-purpose functions (with
% self-documenting names), which call PsychHID, than to call PsychHID
% directly. PsychHID is more general, but also more complicated. However,
% if you acquire a novel input/output device that is HID-compliant, you can
% access it through PsychHID. PsychHID can distinguish between multiple
% devices of the same type by their serialNumber or locationID. 
% 
% NOT RESPONDING? If PsychHID is not responding, try quitting and
% restarting MATLAB. We find that this reliably restores normal
% communication. 
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
%  
% For help on a specific PsychHID subcommand, call PsychHID with the
% subcommand suffixed with a question mark, for example:
%  
%   >> PsychHID NumDevices?
%   Usage:
%   numberOfDevices=PsychHID('NumDevices')
%   Return the the number of USB HID devices connected to your computer.
%  
% TestPsychHID shows a list of all the HID-compliant devices.
%  
% PsychHID links against Apples HID Utilities library.  For information on
% how to program HID devices on OS X in C or Objective C see the HID
% utilities project, its companion project, HID Explorer and related:
% http://developer.apple.com/samplecode/HID_Utilities_Source/HID_Utilities_Source.html
% http://developer.apple.com/samplecode/HID_Explorer/HID_Explorer.html
% http://developer.apple.com/samplecode/Hardware/idxHumanInterfaceDeviceForceFeedback-date.html
%  
% OS9 & WIN: ______________________________________________________
%  
% PsychHID does not exist in the OS9 and Win Psychtoolboxes.  The OS9 and
% WIN Psychtoolbox functions that read from user input devices use mex
% files specific to each input device. The OSX Psychtoolbox accesses all
% input devices through one mex file: PsychHID. USB in general and the
% HID-class in particular are supported by Windows, and there is no
% obstacle to writing a version of PsychHID for Windows, calling the
% appropriate Windows API instead of the Mac OS X HID Manager. It's a
% shame that the OSX and Win APIs are different given that the HID class,
% and all of USB, is a platform-independent standard.
% _________________________________________________________________________
%  
% See also: TestPsychHID, Daq, TestDaq, KbCheck, Gamepad, PsychHardware. 
% web http://psychtoolbox.org/usb.html -browser;
