% InstallKinect - Kinect driver installation instructions.
%
% MS-Windows:
%
% 1. Unzip the file Psychtoolbox/PsychContributed/Kinect-v16-WindowsDrivers.zip
%    into a temporary folder, e.g., C:\tmp\KinectDrivers.
%
% 2. Plug in your Kinect box into a USB port via the interface cable.
%
% 3. The Windows hardware setup assistant will start, tell you about new
%    detected hardware and ask you for drivers. Say "No" to the
%    "automatically search for drivers" option, but select to "provide your
%    own drivers". In the next tab, select "browse for drivers", and then
%    select the temporary folder with the unzipped file (from step 1) as
%    driver folder. Press ok.
%
% 4. The driver will be installed from the zip file, the device manager
%    will notifiy you of the new device, then prompt you for installation
%    of another device. Repeat the same procedure from step 3 until no more
%    devices need to be installed. This procedure will repeat three to four
%    times until all drivers are installed, as the Kinect shows up as
%    multiple separate devices (Video camera, Audio soundcard and
%    Kinect motor).
%
% 5. The setup assistant will tell you that your new device is fully
%    operational. Quit the assistant.
%
% 6. Unplug the Kinect, wait a few seconds, plug it in again, just to be
%    sure it is correctly detected.
%
% 7. Now you can start Matlab or Octave and use the Kinect! Try KinectDemo
%    and Kinect3DDemo for a start, then delve into your own Kinect adventures.
%
% The current Kinect low level drivers are still early prototypes, so
% expect occassional bugs or weird behaviour.
%
%
% GNU/Linux:
%
% If you use the Psychtoolbox distribution provided by the NeuroDebian
% project (http://neuro.debian.net) there's nothing to do. It should "just
% work(tm)". Otherwise the following steps are required:
%
% 1. If you have Ubuntu Linux 10.04 LTS or 10.10 installed, open a terminal
% window and type this sequence of commands, providing your administrator
% password as requested.
%
%    a) sudo add-apt-repository ppa:arne-alamut/freenect
%    b) sudo apt-get update
%    c) sudo apt-get install libfreenect
%    d) sudo adduser YOURNAME video
%       --> (YOURNAME) is your user login name!
%    e) Log out and Log in again.
%
% 2. Kinect is now useable from within Matlab or Octave.
%
% This is still early prototype software, expect bugs, bumps and hickups.
%
% Further installation instructions for other systems can be found at
% http://openkinect.org/wiki/Getting_Started
%
% Mac OS/X:
% See the OS/X section at http://openkinect.org/wiki/Getting_Started
%
%
% Licenses: The driver consists of multiple components, which are licensed
% under different free software / open source licenses. The drivers are
% developed and licensed to you by their respective developers, the members
% of the OpenKinect community. See the respective web sites and licenses
% for copyright, authors, credits etc.:
%
% libusb - The underlying USB communication library is licensed under LGPL.
% libfreenect - The Kinect driver is dual-licensed (at your option) under
% GPLv2 or Apache license.
%
% The source code of the Windows version of the Kinect driver and libusb
% can be found after checking out Psychtoolbox's C source code from the SVN
% repository (see "help UseTheSource" for instructions) as a zip file under:
%
% trunk/PsychSourceGL/Cohorts/Kinect-v16-withsource.zip
%
% The source code for libfreenect (Unix aka Linux / MacOS/X) can be found
% under:
%
% web: http://openkinect.org/wiki/Contributing_Code
%
% The source code of libusb-1.0 can be found at:
%
% web: http://www.libusb.org/
%
