% ExperimentalStuff -- Experimental stuff in this release of Psychtoolbox:
%
% The following components of Psychtoolbox have to be considered
% experimental features, even if they are part of a "stable"
% release or an official Psychtoolbox-x.y.z version.
%
% If you use any of these features then you have to be aware that
% they may be incomplete in their implementation, have known issues,
% or their design and programming interface is subject to change
% in the future, so it may require you to adapt your experiment
% code to the changing interface when upgrading to a future release
% of Psychtoolbox.
%
% Usually we only transfer components from the "beta" branch of
% Psychtoolbox to the "stable" branch or to an official release
% if they are proven to be pretty mature and if their programming
% interface has stabilized, requiring no future changes to user code
% after software updates.
%
% A deviation of this rule is made if the features are mature enough
% to be of substantial benefit for many users, but we don't expect
% the features to be stable and polished in the near future, mostly
% due to lack of sufficient time, ressources or interest on the side
% of the authors of these features. The names of the responsible
% developers are listed in case you want to persuade them to work
% harder on that features or even better, volunteer to provide
% some help in improving them.
% 
%
% GETCHAR: (Allen Ingling, Christopher Broussard, Mario Kleiner)
%
% The GetChar implementation in this release is implemented in the
% Java programming language. It is therefore dependent on Matlabs
% built-in Java support. GetChar is supposed to work with all
% Java virtual machines of version 1.4.2 or later. As such it should
% work on all Matlab versions on MacOS-X 10.3.9 and later. On Microsoft
% Windows and GNU/Linux it is supposed to work with Matlab 7.0.0 and
% later, but not on earlier versions of Matlab. GetChar has been tested
% on Matlab 7.1 for M$-Windows and with Matlab on OS-X 10.3.9 and 10.4.7.
% 
% On OS-X, GetChar will only work in Java mode, i.e. it won't work in
% matlab -nojvm mode. On Microsoft Windows, GetChar will also work in
% matlab -nojvm mode by use of a C implementation of GetChar, but it
% will not return any additional information (e.g., timestamps) then.
%
% The time stamps for key presses returned by GetChar are known to be
% not very accurate and not perfectly consistent with timestamps returned
% by KbCheck, KbWait, GetSecs and other PTB functions. For accurate timing
% do not rely on GetChar. It is recommended to run "TestGetChar" before
% first use of GetChar to verify its proper working and to get a feeling
% for the (in)accuracy of its timestamps. Read "help GetChar" and in case
% of trouble, read "help PsychJavaTrouble".
%
% We generally recommend using KbCheck and KbWait instead of GetChar if
% possible, as these functions are well tested and known to be robust,
% whereas GetChar is known to be one of the most fragile and troublesome
% functions in Psychtoolbox.
%
% iViewXToolbox: (Frans Cornelissen)
%
% The iViewXToolbox should allow to control iView Gazetrackers from Matlab.
% The current version is a very early prototype, most likely not yet functional,
% just included for backup purpose.
%
% PSYCHSOUND: (Mario Kleiner)
%
% PsychSound is supposed to be a well working replacement for the Snd
% function in the far future. As of now, it only works on MacOS-X for
% PowerPC computers and it is only useable for capture of sound input,
% not for sound playback. Sound capture works well, but the interface
% is subject to change in the future, so code that makes use of PsychSound
% may need to be slightly rewritten in the future.
%
% EYELINKTOOLBOX: (Frans Cornellisen, Christopher Burns and others)
%
% The version of the Eyelink toolbox bundled with this release is a
% working beta version for OS-X on PowerPC computers and for Microsoft
% Windows.
%
% It should work well for basic control and use of the Eyelink eyetracker, but some
% of the more advanced features are missing, the help texts are not
% yet fully updated and the demos are in need for cleanup. The toolbox
% is not yet available for GNU/Linux or MacOS-X on the new IntelMacs.
% Availability of OS-X IntelMac support will depend on the future
% availability of runtime libraries from SR-Research systems.
%
% MOGL OpenGL for Matlab support: (Richard F. Murray, Mario Kleiner)
%
% The OpenGL for Matlab low-level functions, as far as they are available,
% work very well for many applications, but a few OpenGL functions are
% not yet supported or incomplete. Look into the PsychOpenGL/MOGL/wrap/
% folder for a listing of all OpenGL functions. Functions prefixed with
% an underscore are not yet enabled. Given the enormous size and complexity
% of the OpenGL API we don't have any ressources to validate all functions
% for their correctness, but MOGL is used for pretty complex projects by
% some of the developers, so most common functions are known to be bug-free
% and bugs are quickly fixed if we find them.
%
% PsychHID and the DAQ toolbox: (Denis Pelli)
%
% PsychHID is known to work perfectly well for keyboard queries, but the
% functions for low-level USB-HID access are known to have some unresolved
% stability issues on some systems as of 6.10.2006. The DAQ toolbox depends
% on these functions and therefore may suffer from problems on some setups.
% Also the DAQ toolbox itself has to be considered beta quality, so prepare
% for a slightly bumpy ride if you use it.
%
% MICROSOFT WINDOWS version of the OpenGL Psychtoolbox: (Mario Kleiner)
%
% The Windows version of PTB is a hybrid of the old Windows PTB 2.54 and
% parts of the new OS-X PTB. All M-Files are identical to the OS-X version.
% The "Screen" command is nearly identical to the OS-X version, as well as
% the ShowCursor, HideCursor, GetMouse, SetMouse and KbName commands.
% Many other Mex files are taken from the old PTB. As far as functions are
% implemented, they are considered to be identical in functionality, behaviour
% and quality as their OS-X counterparts, unless otherwise noted in the
% help of the functions. There are gaps and deviation though:
% 
% The very sophisticated FONT functions for query, manipulation and handling
% of fonts are only present in the OS-X version. They are non-existent in
% the Windows version, without any plans to implement them. The Screen('DrawText')
% command provides less options to customize the appearance of fonts, the
% quality of font rendering is slightly lower, but text drawing speed is
% much faster. The windows version only supports 8-Bit ASCII characters,
% whereas OS-X provides the full Unicode character set.
%
% PsychHID is missing, so the DAQ toolbox and other toolboxes based on PsychHID
% are not available on Windows. It wouldn't be difficult for an experienced
% C programmer to implement PsychHID though. Any volunteers?
% 
% PsychSound is missing.
%
% CLUT animation does not work on Windows due to some brain-damage in the
% design of the M$ operating system. There's nothing we could do about that.
% Setup of static color lookup tables (gamma tables) for the purpose of display
% calibration is supported though.
%
% Multidisplay support is implemented in a slightly different behaviour due to
% differences in the way OS-X and Windows handle multiple displays. They are
% equivalent in functionality, but the assignment of screen numbers to displays
% is different, so you will need to take this into account if you want to
% write portable scripts that use this feature.
%
% There are more differences, but most common experiment scripts should work
% identically on OS-X and Windows.
%
%
% GNU/LINUX version of Psychtoolbox: (Mario Kleiner)
%
% The GNU/Linux port of PTB is more incomplete than the Windows version.
% Everything mentioned in the M$-Windows section applies, except for CLUT
% animation capabilities which are excellent and Screen('DrawText') support
% which is very basic, only allowing for a pretty limited number of font types
% and font attributes to choose from. Quicktime movie playback support is
% completely missing due to lack of support for Apples Quicktime SDK. Sound
% output is available but implemented in a very basic way.
% The most excellent areas are reliability of timing and video capture with
% Firewire cameras. Timing is excellent due to Linux superior support for
% accurate scheduling. Same goes for video capture functionality via Firewire.
% The Linux version currently only runs on Matlab version 6.x, support for
% later Matlab releases and for a more complete feature set will depend on
% public interest. The Linux version is mostly used for my own projects, so
% the strong areas are the ones for which i have an immediate need.
%
% GNU/Octave support: (Mario Kleiner)
%
% Psychtoolbox can be used with GNU/Octave on Linux instead of Matlab, allowing
% for complete independence from proprietary/commercial software. Ports for
% OS-X and Windows would be feasible with little amount of work, but there is
% no intention to do this in the close future due to lack of time und uncertainty
% about the amount of general interest.
%
%
%
% There are probably a few more incomplete areas, but these are the ones i am
% aware of. If in doubt, ask on the Psychtoolbox forum:
% http://www.psychtoolbox.org/forum.html
%
% Good luck
% Mario Kleiner, 6.10.2006.
