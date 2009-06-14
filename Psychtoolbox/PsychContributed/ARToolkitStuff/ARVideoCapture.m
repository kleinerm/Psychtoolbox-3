% ARVideoCapture
%
% How to install, setup and use the ARVideo video capture engine.
%
% Overview and Introduction:
%
% The ARVideo video capture engine is one of multiple video capture engines
% supported by Psychtoolbox's Screen() command. This engine internally uses
% the video capture library implemented by the free software "ARToolkit", a
% free toolkit for augmented reality applications, licensed under GPL v2
% license and included in Psychtoolbox.
%
% The ARVideo engine has the engineId 2. On MS-Windows operating systems,
% this is the default engine used for video capture if no other engine is
% explicitely selected. You can change the global default selection of a
% videocapture engine via the command
%
% Screen('Preference', 'DefaultVideoCaptureEngine', engineId);
%
% anywhere in your scripts. The setting will affect the next
% Screen('OpenVideoCapture',...); command and override the built-in default
% engine selection for your operating system.
%
% You can also select the engine on a case by case basis by passing the
% optional 'engineId' parameter to the Screen('OpenVideoCapture',..., engineId);
% command when opening a video device.
%
% ARVideo uses different capture engines under the hood, depending on the
% operating system you're running:
%
% On GNU/Linux, it uses the open source GStreamer multi-media framework to
% access almost any kind of video source supported on the Linux OS. You'll
% need to have GStreamer installed on your Linux system for this to work.
%
% On Mac OS/X, it uses the Quicktime Sequencegrabber engine, just as the
% default Quicktime engine (engineId 0) does on OS/X, so most of the time
% you will want to use the OS/X default engine instead.
%
% On MS-Windows, where ARVideo is the default engine, it uses the Microsoft
% Windows DirectShow multi-media framework. As pretty much all video
% capture devices for Windows come with DirectShow drivers, this should
% allow to access almost any video capture device that is supported on the
% Windows platform.
%
% Installation on MS-Windows:
%
% On MS-Windows, you'll need to install a few special runtime DLL libraries
% before you can use the video capture engine (or other parts of ARToolkit,
% e.g., the ones supported by PsychCV()). For that you need to open the
% Psychtoolbox subfolder Psychtoolbox/PsychContributed/ARToolkitStuff/ ,
% e.g., via the Windows explorer. Then you'll need to copy a few of the
% .dll files inside that folder into one of the following locations in your
% system: Either the Matlab DLL installation directory - that is the
% directory inside your Matlab folder which also contains the Matlab main
% executable "Matlab.exe" and a host of other libraries (files ending with
% the extension .dll or .DLL). Another option is to copy these files into
% the main system library folder, e.g., C:\Windows\system32\ on most XP or
% Vista systems,  or C:\WINNT\system32\ on older Windows 2000 systems.
% You will likely need administrator privileges to copy the DLL files
% there.
%
% On some systems, it is sufficient to copy the files libARvideo.dll and
% DSVL.dll to one of the target folders. If the video capture engine
% doesn't work and aborts with some error message about failing to load
% DLL's, you'll also need to copy the Microsoft Visual C runtime libraries
% msvcp71.dll and msvcr71.dll as well.
%
% Another prerequisite is that you have the Microsoft DirectShow runtime
% and proper drivers for your video capture device installed, but this can
% be almost taken for granted on any Windows 2000 system or later.
%
%
