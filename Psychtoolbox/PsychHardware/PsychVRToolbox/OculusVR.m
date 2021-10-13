% OculusVR - How to setup the drivers for Oculus VR products.
%
% Psychtoolbox supports Virtual Reality Head mounted displays,
% (VR-HMDs) and some other hardware from Oculus VR LLC.
%
% Note: Oculus VR's trademarks, e.g., Oculus, Oculus Rift, etc.
% are registered trademarks owned by Oculus VR, LLC.
%
% Oculus Rift CV-1 and later models from Oculus on Windows:
% ---------------------------------------------------------
%
% The more recent Oculus Rift CV1 and Oculus Touch controllers, or more
% generally, Oculus VR hardware supported by the Oculus SDK and Oculus
% runtime v1.16 and later can be utilized with the newer PsychOculusVR1()
% driver. This requires use of an operating system and hardware platform
% supported by the Oculus runtime v1.16 or later. Currently the
% PsychOculusVR1() driver is known to work successfully on 64-Bit Microsoft
% Windows 7 and later for 64-Bit Intel compatible processors, with suitable
% graphics cards from AMD and NVidia. Use of the latest stable version of
% Windows 10 is recommended, as the driver is best tested on that system and
% Windows 10 is supposed to provide best VR performance and quality due to
% operating system features not present in earlier Windows versions. You will
% need to have the Oculus VR runtime version 1.16 or later installed and setup
% correctly for the driver to work.
%
% Oculus Rift CV-1 and later models from Oculus on Linux:
% -------------------------------------------------------
%
% Limited experimental support for the Oculus Rift CV1 on Linux is available
% by use of the PsychOpenHMDVR() driver, which utilizes OpenHMD to drive the
% device. Functionality and display quality is substantially limited compared to
% use of the PsychOculusVR1() driver though. See "help PsychOpenHMDVR".
%
% Oculus Rift DK-1 and DK-2 on Linux, macOS, Windows:
% ---------------------------------------------------
%
% Currently the older Oculus Rift DK1 and Oculus Rift DK2 are supported
% by Psychtoolbox on the 64-Bit editions of GNU/Linux, Apple OSX and
% Microsoft Windows-7 and later, both under 64-Bit GNU/Octave and
% 64-Bit Matlab by use of the PsychOculusVR() driver. This by use of
% the Oculus v0.5 SDK for these operating system platforms.
%
% In order to use such a HMD you need to install the Oculus VR
% runtime version 0.5.0.1 (-beta) for your operating system.
% This will not work with earlier or later versions of the runtime!
%
% The sdk and runtime for Linux is part of the Psychtoolbox GitHub and can be
% downloaded from the following folder:
%
% https://github.com/Psychtoolbox-3/Psychtoolbox-3/tree/master/PsychSourceGL/Cohorts/OculusSDK
%
% Due to filesize limits of GitHub unfortunately we weren't able to bundle the SDKs and
% runtimes for macOS and Windows in the same way. In the past those runtimes could be
% downloaded from:
%
% https://developer.oculus.com/downloads
%
% You had to select "Legacy", and the version as "0.5.0.1-beta"
% in the drop down menus. Then download the "Oculus Runtime for OSX",
% "Oculus Runtime for Windows", or "Oculus SDK for Linux (experimental)"
% and follow the installation and setup instructions contained in those
% installers / SDKs.
%
% Unfortunately this option no longer exists as of July 2021, so if you need those
% runtimes, you have to go on a hunt on the internet or maybe check on the Psychtoolbox
% forum if some kind user still would have a copy of it for you.
%
% Once the 0.5.0.1 runtime is installed according to instructions, and the HMD is
% properly connected via HDMI video and the USB ports for headset and tracking
% camera (in case of the Rift DK2), Psychtoolbox should detect the HMD and
% make use of it.
%
% Display setup for Rift DK-1/DK-2 with the legacy 0.5.0.1 runtime on Windows:
% ----------------------------------------------------------------------------
%
% On Windows this works with "Extended Desktop" mode.
%
% Display setup for Rift DK-1/DK-2 with the legacy 0.5.0.1 runtime on Linux/X11,
% or for the Oculus Rift CV-1 and later with the OpenHMD runtime on Linux/11:
% ---------------------------------------------------------------------------
%
% On Linux it works on a regular dual display setup, if your machine uses a Linux
% kernel of version 4.15 or earlier. On a modern distribution with a later kernel,
% or generally for best performance and timing you should set up a separate X-Screen
% for the Rift display. The Psychtoolbox/PsychHardware/LinuxX11ExampleXorgConfs
% subfolder contains an example xorg.conf files for setting up dual-X-screen setups
% for Linux, with the GUI on X-Screen 0 and the Rift on X-Screen 1. You will need
% to adapt the example file to your specific setup though. Running without a desktop
% compositor or the desktop compositor disabled may give another performance boost
% on Linux with not so high end cards.
%
% Our current experience shows that the Rift DK2 works best for more
% complex 3D VR scenes on Linux or Windows. Performance on OSX was much
% lower and the setup less reliable in general, as most things on macOS.
%
% If you want to write portable code that makes use of VR headsets, and
% not only of the Oculus Rift, but also future headsets from Oculus or
% from other vendors, then use only the functions of the PsychVRHMD()
% driver. It is expected to work also on future HMDs and HMDs of other
% vendors, once they become available. For use of Oculus Rift specific
% features you may need to use the PsychOculusVR() driver directly, but
% for most usage scenarios that should not be neccessary.
%
% Before use of the HMD, please read the "Health and safety instructions"
% carefully. They are included in the Psychtoolbox/PsychDocumentation/
% folder as file Oculus_Health_and_Safety_Warnings.pdf. This file is
% provided as part of the Oculus SDK runtime and included here for
% your convenience.
%
% The Oculus Rift runtime also comes with a setup tool named "RiftConfigUtil"
% that allows to adapt the rendering of stimuli to the properties of your
% subject, e.g. the interpupillary distance between both eyes.
%
%
% Some demos to get you started:
% ------------------------------
%
% VRHMDDemo -  Shows how to use a HMD as a regular monoscopic display
%              monitor via VRHMDDemo(0), or as a stereoscopic display
%              via VRHMDDemo(1), or simply VRHMDDemo. As you can see,
%              only one line of code is needed to setup a single HMD
%              properly as monoscopic or stereoscopic monitor "strapped
%              to the subjects head". Mono and stereo stimulus presentation
%              is done like in any other mono or stereo Psychtoolbox script.
%
% Various PTB stereo demos demonstrate use of HMDs, e.g, ImagingStereoDemo(103)
% for a random dot stereo display, ImagingStereoMoviePlayer(moviefile, 103) for
% playback of stereoscopic movies.
%
% The most interesting use is use of the HMDs head tracking to render complex 3D
% stimuli via OpenGL which can be navigated by the subjects head movements.
% Enabling an existing 3D stereoscopic script to use the HMD and make use of head
% tracking is a task that requires only a few more lines of code and can be
% accomplished in less than 15 minutes. The following demos demonstrate this use
% of the HMD: MorphDemo for rendering a morphing 3D object, SuperShapeDemo for
% rendering a complex organic "SuperShape", VRHMDDemo1 for rendering of the
% spinning Utah teapot in 3D, with head movement and mouse control of observer
% location and looking direction. VRHMDDemo1 also shows a few optimizations to
% get best performance for more complex scenes.
%
% More such demos will follow in the future.
%
% If you want to immerse your subjects in realistic 3D worlds, you can
% do this by use of the Horde3DForPsychtoolbox toolbox which allows you
% to use the Horde3D graphics engine with Psychtoolbox. That toolbox also
% supports VR HMDs, as demonstrated in the demo named "HordeVRHMDDemo1",
% which immerses you in a complex 3D VR scene and allows you to navigate
% it via mouse movements and head movements.
%
% You can get the Horde3DForPsychtoolbox addon toolkit from GitHub as
% git checkout or as a downloadable zip file:
%
% https://github.com/kleinerm/Horde3DForPsychtoolbox
%
% More such demos for full VR environments as part of Horde will follow
% in the future.
%
% Enjoy your immersion into new (virtual) worlds!
