% Psychtoolbox/PsychHardware/Daq/
% The Daq Toolbox
% Control the USB-1208FS or USB-1608FS (see below) data acquisition device. 
%
% The Daq Toolbox is a set of MATLAB functions providing communication with
% a particular USB data acquisition device (daq): the USB-1208FS made by
% Measurement Computing (see URL below). This daq costs $150 and offers "50
% kHz" input and output 12-bit sampling of analog voltages (8 in, 2 out)
% and 16 digital i/o lines, with signals brought out to screw terminals.
% ("50 kHz" is a theoretical upper limit: as of 18 April 2005 we attain 2
% kHz. See DaqTest.) The USB-1208FS is the size of a wallet and is powered
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
% DaqTest.m (in the PsychTests folder). The only requirements are MATLAB,
% Mac OS X 10.3 or better, and a USB port to connect the USB-1208FS. 
% web http://psychtoolbox.org/daq.html -browser; [I believe the previous
% statements are not quite correct.  There are calls to GetSecs and
% WaitSecs sprinkled through the code, and these functions are both part of
% the Psychophysics Toolbox distribution.  I have also added a bit more
% dependency in the DaqTest function.  In addition to the files mentioned,
% if you want to run the Daq toolbox without the PsychToolbox you will also
% need copies of get_color.m, ConfirmInfo.m, and TwoStateQuery.m.  These
% are functions I wrote for myself and have tinkered with quite a bit over
% the years (except get_color which hasn't really changed since 1994), and
% I just find them too useful to do without.  --  MPR 1/11/08]
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
% Toolbox as a model.  [I have partially broken the isolation, but it is a
% minimal break.  For a couple of reasons -- primarily because you can set
% gains using DaqSetGainQueue but you can't get (i.e. poll) the gain queue,
% I found it useful to create a preferences file.  The location I chose to
% store that file uses the fileseparators and an expectation that the file
% structure of your machine is that of Mac OS X.  It would be pretty simple
% to modify that for your own useage; search for the string:
%
% ~/Library/Preferences/
%
% and replace it and other subdirectories with appropriate strings.  You
% will find the string in DaqAIn, DaqAInScan, DaqALoadQueue, and
% DaqCalibrateAIn. -- MPR 1/11/08]
% 
% NOT RESPONDING? If PsychHID is not responding, e.g. after unplugging it 
% and plugging it back in, try quitting and restarting MATLAB. We find that
% this reliably restores normal communication. 
% 
% Denis Pelli, 30 April 2005.
%
% From November 2007 through January 2008, the functions in this toolbox 
% were tested with a USB-1608FS.  There are significant hardware
% differences between the two devices, notably one Digital I/O port vs. two
% and no analog output ports in the 1608.  Nevertheless, most of the
% command codes are the same, so the two devices can share a lot of common
% software.  The USB-1608FS was connected to a Mac Pro running Leopard for
% most tests (some tests also run with Tiger; no significant performance
% differences were found in those cases).  If you have a 1608 and have
% trouble running any of the code here, you might try e-mailing
% MickeyPRowe@gmail.com.
%
% Mickey P. Rowe, 10 January 2008.
% 
% web http://www.measurementcomputing.com/cbicatalog/directory.asp?dept_id=403 -browser;
% web http://psychtoolbox.org/daq.html -browser;
% See also: DaqFunctions, DaqTest, PsychHIDTest, PsychHID,
% PsychHardware, DaqPins, DaqCalls, DaqCodes,
% DaqDeviceIndex, DaqFind, DaqDIn, DaqDOut, DaqAIn, DaqAOut, DaqAInScan, DaqAOutScan.
