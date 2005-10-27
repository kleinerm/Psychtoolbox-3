% Psychtoolbox:PsychHardware:Daq:
% The Daq Toolbox
% Control the USB-1208FS data acquisition device. 
%
% The Daq Toolbox is a set of MATLAB functions providing communication with
% a particular USB data acquisition device (daq): the USB-1208FS made by
% Measurement Computing (see URL below). This daq costs $150 and offers "50
% kHz" input and output 12-bit sampling of analog voltages (8 in, 2 out)
% and 16 digital i/o lines, with signals brought out to screw terminals.
% ("50 kHz" is a theoretical upper limit: as of 18 April 2005 we attain 2
% kHz. See TestDaq.) The USB-1208FS is the size of a wallet and is powered
% through its USB cable. We have complete control of it from within Matlab,
% via the PsychHID extension. 
% 
% There is a Daq M file (see DaqFunctions) for each USB-1208FS firmware
% command, plus a few more to facilitate use of the device. 
% 
% The Daq Toolbox is free and can be downloaded from the Daq Toolbox web
% page, below. It is self-contained, and may be used with or without the
% rest of the Psychtoolbox. The Daq Toolbox consists of the Daq folder of M
% files and the PsychHID MEX file (both in the PsychHardware folder) and
% TestDaq.m (in the PsychTests folder). The only requirements are MATLAB,
% Mac OS X 10.3 or better, and a USB port to connect the USB-1208FS. 
% web http://psychtoolbox.org/daq.html -browser;
% 
% There is a near-perfect isolation of the dependency on platform and
% device at the two levels of code in the Daq Toolbox. The PsychHID MEX
% file (written in C) is highly dependent on the platform, Mac OS X, but
% independent of the particular HID-compliant device. It provides generic
% HID commands. (HID, or Human Interface Device, is a USB class specifying
% a communication protocol for the device and host.) The Daq M files are
% specific to our HID-compliant device, the USB-1208FS, but independent of
% the platform (Mac OS X), and would run unchanged in MATLAB on any other
% computer for which we provided the PsychHID extension. We hope that users
% of the OSX Psychtoolbox will find it easy to write new MATLAB M files
% using PsychHID to support other HID-compliant devices, using the Daq
% Toolbox as a model.
% 
% NOT RESPONDING? If PsychHID is not responding, e.g. after unplugging it 
% and plugging it back in, try quitting and restarting MATLAB. We find that
% this reliably restores normal communication. 
% 
% Denis Pelli, 30 April 2005.
% web http://www.measurementcomputing.com/cbicatalog/directory.asp?dept_id=403 -browser;
% web http://psychtoolbox.org/daq.html -browser;
% See also: DaqFunctions, TestDaq, TestPsychHID, PsychHID,
% PsychHardware, DaqPins, DaqCalls, DaqCodes,
% DaqDeviceIndex, DaqDIn, DaqDOut, DaqAIn, DaqAOut, DaqAInScan,DaqAOutScan.
