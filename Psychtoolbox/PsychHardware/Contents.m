% Psychtoolbox:PsychHardware
% Sense and control the world outside the computer. For keyboard and mouse 
% see PsychBasic. For other USB devices, see
% web http://psychtoolbox.org/usb.html -browser;
%
%   help Psychtoolbox % For an overview, triple-click me & hit enter.
%
% ActiveWire          - Win: ActiveWire minimal USB interface, $59.  
%                            web http://www.activewireinc.com/ -browser ;
% CMCheckInit         - OS9: Turn on color meter.
% CMClose             - OS9: Turn off color meter.
% MeasSpd             - OS9: Use color meter to measure a spectral power 
%                            distribution.
% MeasXYZ             - OS9: Use color meter to measure XYZ coordinates.
% CRSColorToolbox     - OS9: Talk to CRS Colorimeter.
% PR650Toolbox        - OS9: Talk to PR650 Colorimeter. (This will
%                            eventually supercede PsychObsolete:CMETER.)
% Daq folder          - OSX: Control the USB-1208FS data acquisition
%                            device.
% Joystick            - OS9: Read USB game controllers. OSX: renamed "Gamepad".
% Gamepad             - OSX: Read USB game controllers. Formerly "Joystick".
% GetGamepadIndices   - OSX: Get indices of gampads in PsychHID device list.
% GetKeyboardIndices  - OSX: Get indices of keyboards in PsychHID device list.
% GetMouseIndices     - OSX: Get indices of mice in the PsychHID device list.
% PsychHID            - OSX: Read/write any HID-compliant USB device. See web page:
%                            web http://psychtoolbox.org/usb.html
% PsychSerial         - Win, OS9: Send and receive through serial ports.
% SERIAL              - Win: Send and receive through serial ports. 
%                            Standard Matlab function. Requires Java, so it's 
%                            not available if you run Matlab with the
%                            -NOJVM switch. In Win we use the -NOJVM switch 
%                            to fix the problem with GetChar.
% COMM                - OSX  Send and receive through serial ports.
%                            web http://www.mathworks.com/matlabcentral/fileexchange/loadFile.do?objectId=4952&objectType=file -browser;
% Snd                 - Win, OS9, OSX: Play a sound.
% TestIOPort          - OSX: Use the Code Mercenaries' IO Warrior minimal
%                            USB interface, 49 euros. "IOPort.mexmac" should be  
%                            renamed "IOWarrior.mexmac".
%                            web http://www.codemercs.com/E_index.html -browser;
% 
% Minolta Toolbox
% Set of m-files and sample code that can be used for reading luminance data
% from the Minolta LS-110 luminance meter. Contains simple program for 
% creating a LUT.
% web http://cornelis.med.rug.nl/pub/psychtoolboxstuff/minoltatoolbox.sit -browser;
% 
% OSX: For serial port access under Mac OS X you may want to download COMM
% from the Mathworks web site. Written by Tom Davis. "This MEX-file
% provides Matlab access to Mac serial ports. This includes built-in ports 
% (modem and irDa) and add-on ports, such as USB-to-serial adapters. It has
% been tested with Mac OS X 10.3 and Matlab 6.5.1. The Xcode project used to
% develop the MEX-file is included."
% web http://www.mathworks.com/Matlabcentral/fileexchange/loadFile.do?objectId=4952&objectType=file -browser;
% 
% TCP/UDP/IP Toolbox 2.0.5
% This toolbox can be used to set up TCP/IP connections or send/receive 
% UDP/IP packets in Matlab. It can transmit data over the Intranet/Internet 
% between Matlab processes or other applications. It is possible to act as 
% server and/or client and transmit strings, arrays of any data type, 
% files, or Matlab variables. There's no mexmac file in this release, 
% but this toolbox works perfectly on Mac OS X as is. Tested with 
% Matlab 7.0.1 and Mac OS X 10.3.7. Just use the MEX command with the main 
% pnet.c file and it will generate a pnet.mexmac.
% web http://www.mathworks.com/Matlabcentral/fileexchange/loadFile.do?objectId=345&objectType=file -browser;
% 
% See also PsychHID, Daq, Gamepad.
