% OpenXR - How to set up and use Psychtoolbox OpenXR support.
%
% Since Psychtoolbox version 3.0.19, Psychtoolbox supports the modern
% Khronos open, cross-platform, cross-vendor, cross-device OpenXR api for
% implementing (V)irtual (R)eality VR, (A)ugmented (R)ealtiy AR, and/or
% (M)ixed (R)eality MR applications, summarized under the umbrella term
% e(X)tended (R)eality XR, hence the name OpenXR. See:
% https://www.khronos.org/openxr
%
% Unless otherwise specified by a users script, the PsychVRHMD('AutoSetupHMD')
% function from now on will try to use a system installed OpenXR runtime to
% run VR applications, with fallbacks to the older legacy drivers like
% PsychOculusVR1 for recent Oculus devices on MS-Windows, PsychOculusVR for
% Oculus Rift DK-1/DK-2 on Linux/X11 and MS-Windows, or to PsychOpenHMDVR
% on Linux/X11. The driver is designed to be reasonably backwards compatible,
% so most scripts should continue to work unmodified "plug & play".
%
% In the current release, we support the XR subset of VR virtual reality
% applications by use of any VR Head mounted display (VR-HMD) and for VR
% input devices which are supported by a OpenXR 1.0 compliant runtime that
% provides the following minimum set of OpenXR 1.0 extensions:
%
% - All: XR_KHR_opengl_enable, XR_EXT_debug_utils, and XR_KHR_composition_layer_depth.
% - Additionally on MS-Windows: XR_KHR_WIN32_convert_performance_counter_time.
% - Additionally on Linux/Unix: XR_KHR_convert_timespec_time.
% - Optional, but not tested without it: XR_FB_display_refresh_rate.
% - Optional, for improved input controller support: XR_EXT_hp_mixed_reality_controller,
%   XR_HTC_vive_cosmos_controller_interaction, XR_HTC_vive_focus3_controller_interaction,
%   XR_KHR_binding_modification, XR_EXT_dpad_binding.
% - Optional, for basic eye gaze tracking: XR_EXT_eye_gaze_interaction.
%
% On VR HMDs with a builtin eye gaze tracker, our OpenXR driver is also
% capable of using such hardware to track and report eye gaze. Basic eye
% gaze tracking should work with any HMD and OpenXR runtime that supports
% the optional OpenXR XR_EXT_eye_gaze_interaction extension. This allows
% reporting of a "cyclops eye" gaze vector and 2D gaze position, possibly
% synthesized from separate eye gaze measurements on a binocular eye
% tracker, or the single measurement from a monocular eye tracker.
% Additionally, on MS-Windows only, with certain HTC HMDs only, more
% advanced eye gaze tracking is supported via HTC's SRAnipal eye tracker
% runtime on suitable HMDs, e.g., the "HTC Vive Pro Eye". This allows
% additionally for binocular eye gaze reporting, reporting of eye opening,
% and estimated eye pupil diameter. Some demos have support and demonstration
% of eye gaze tracking. E.g., GazeContingentDemo.m for gaze contingent
% manipulation of 2D monoscopic stimuli, VRHMDDemo.m for 2D tracking and
% visualization of gaze wrt. monoscopic or stereoscopic stimuli.
% VRInputStuffTest.m shows gaze tracking wrt. 3D VR stereoscopic scenes,
% both showing 2D gaze position and 3D gaze rays. VREyetrackingTest.m
% allows to test and assess various aspects of 2D gaze tracking of 2D or 3D
% stimuli presented monoscopically or stereoscopically.
%
% So far successfully tested with the PTB 3.0.19.5/3.0.19.6 release are:
%
% - The open-source Monado(XR) runtime version 21.0.0 for Linux/X11, as shipping
%   with Ubuntu 22.04-LTS and later, or as a 3rd party ppa for Ubuntu 20.04-LTS,
%   as well as part of Debian GNU/Linux 12/unstable/testing. See the following link
%   for more information about Monado:
%
%   https://monado.freedesktop.org
%
%   This has been tested on Ubuntu 20.04.6-LTS and 22.04-LTS with AMD and NVidia
%   gpu's so far.
%
% - The proprietary Valve SteamVR runtime version 1.24.7 and 1.25.7 on Linux
%   (Ubuntu 20.04.6-LTS and 22.04.3-LTS), and SteamVR 1.25.7 and 2.0 on Microsoft
%   Windows 10 21H2 - 22H2.
%
% - The proprietary OculusVR runtime version 1.81.0 on Microsoft Windows 10
%   21H2 - 22H2.
%
% Testing so far occured with a OculusVR Oculus Rift CV-1 HMD with 2 Oculus
% tracking cameras and 2 Oculus touch controllers, as well as a Oculus
% Remote control, and a Microsoft XBox 360 gamepad controller. Additionally
% testing was performed under Monado and SteamVR with a HTC Vive Pro Eye
% and its two "Vive Wand" controllers, tracked by a Valve Lighthouse
% version 2 system with two Vive lighthouse emitter stations.
%
% Eye gaze tracking was tested under Windows 10 22H2 under SteamVR 1.25.6
% and and SteamVR 2.0 and HTC SRAnipal SDK 1.3.6.8, under Octave 7.3 and
% Matlab R2022b and R2023b, using a HTC Vive Pro Eye VR headset. Both OpenXR
% "cyclops" gaze tracking, and also binocular gaze tracking with eye opening
% and pupil diameter reporting worked well.
%
% A limitation of the current OpenXR spec is that it doesn't provide any
% means for reliable, robust, trustworthy, accurate and precise visual
% stimulus onset timestamping. We are investigating a future solution for
% reliable and trustworthy timestamping for the open-source MonadoXR
% runtime on Linux and hope to find a solution there in the foreseeable
% future, stay tuned. For now, as of Psychoolbox 3.0.19.6 we have a hack
% that only works on a modified version of Monado + a modified version of
% Mesa, on AMD or Intel gpu's under Linux. The hack has various restrictions
% and it impacts performance. It is also not quite plug and play to set up.
% See 'help PsychOpenXR', the section about the Monado metrics hack, for
% instructions.
%
% Testing also showed that all tested proprietary OpenXR runtimes, ie.
% both OculusVR and SteamVR, violate the OpenXR specs stimulus timing
% requirements, as of December 2023. The only exception was the open-source
% Monado(XR) runtime for Linux.
%
% The same limitations are true for the old OculusVR runtimes on
% MS-Windows. To get at least approximately correct timestamps, the driver
% therefore will switch to a multi-threaded mode of operation if it detects
% the need for timestamping or timing, or if that need is specified with
% new 'basicRequirements' keywords to PsychVRHMD('AutoSetupHMD') or to
% PsychVRHMD('SetupRenderingParameters'), e.g., 'TimestampingSupport' for
% timestamps, or 'TimingSupport' for onset timing. The switch to
% multi-threading will cost some performance and possibly introduce extra
% latency. In the case of SteamVR on MS-Windows it may even cause bugs and
% hangs in some cases. Therefore additional keywords like
% 'NoTimestampingSupport', 'NoTimingSupport', or 'ForbidMultiThreading'
% allow your script to specify also if it doesn't need precise timing or
% timestamping or does not want multi-threading to be used.
%
% Testing showed that MonadoXR was the most reliable and bug-free runtime,
% whereas both OculusVR and SteamVR exposed various other serious bugs. Our
% driver tries to work around such known bugs on those runtimes, sometimes
% by use of multi-threading, which impacts performance. Therefore various new
% keywords beyond the ones mentioned above exist to control these
% quality/reliability vs. performance tradeoffs for your specific script
% and paradigm.
%
% 'help PsychVRHMD' lists those new keywords in the section for
% 'AutoSetupHMD', and the 'help PsychOpenXR' sometimes gives more detailed
% infos.
%
% If you need precise timing at all costs, potentially to the detriment of
% most other functionality, performance or quality, there is also the
% keyword 'TimingPrecisionIsCritical' to specify, in addition to the other
% timing/timestamping keywords. This keyword will force the selection of
% the driver with the highest possible timing precision/reliability. At the
% moment this means to probe for the PsychOculusVR driver for the old
% Oculus v0.5 runtime for Linux/X11 and MS-Windows, only usable for the
% original Oculus Rift developer kits DK-1 and DK-2. Then a fallback to a
% potentially timing enhanced MonadoXR implementation, once such a thing
% exists. Then back to standard OpenXR as a last resort measure, in which
% case timestamps will not be reliable or trustworthy at all!
%
%
% Basic Setup:
% ============
%
% MS-Windows:
% -----------
%
% - Oculus: If you bought and set up a Oculus HMD, then the OculusVR-1
%   OpenXR runtime will have been installed and setup already and should
%   just work(tm) with modern Oculus devices like Rift DK1/DK2/CV1/S and
%   the new Meta Quest devices and associated controllers.
%
% - SteamVR: The same should be true for SteamVR supported HMDs if you
%   followed the setup instructions, e.g., for the Valve Index HMDs or
%   early HTC Vive HMDs, including the Vive Pro and Vive Pro Eye.If you
%   chose a (W)indows(M)ixed(R)eality WMR-HMD, you need to install SteamVR
%   and set it up as OpenXR runtime for those HMDs, as the Microsoft
%   Windows built-in WMR OpenXR runtime does not support OpenGL interop, so
%   SteamVR is needed as a middle-man and translator between Psychtoolbox
%   OpenGL rendering and WMR's Direct3D only rendering.
%
%   If you want to use binocular eye tracking via SRAnipal on HTC HMDs with
%   eye tracker, the following URLs may provide useful setup and getting
%   started instructions:
%
%   - HTC official guide: https://developer.vive.com/resources/openxr/openxr-pcvr/tutorials/how-use-sr_runtime
%   - Much more detailed webpage, very well done by some motivated volunteer:
%     https://github.com/corycorvus/Eye-Tracking-Wiki
%
% - Other OpenXR runtimes exist from HTC for their latest devices, or from
%   other vendors like Varjo for their devices.
%
% Linux/X11:
% ----------
%
% - MonadoXR is provided via 'sudo apt install monado' on Ubuntu 22.04-LTS
%   and later, Debian GNU/Linux 11 and later, and probably other distros.
%   Also as a 3rd party ppa for Ubuntu 20.04-LTS, but we now recommend
%   using at least Ubuntu 22.04-LTS. Note that for some HMDs, e.g., the
%   Oculus Rift CV-1, you also need OpenHMD, and potentially build Monado
%   from source against OpenHMD. See the "supported hardware" section on
%   Monado's website, for natively supported devices, and for devices that
%   additionally need OpenHMD, and potentially building Monado from source
%   code against an installed libOpenHMD. The HTC Vive devices do have
%   basic 3 degree of freedom orientation tracking support in Monado, but
%   for full 6 degrees of freedom tracking you will need to install
%   libsurvive and compile Monado from source against libsurvive, as an
%   external more capable tracking library at the moment.
%
% - SteamVR can be installed to use SteamVR supported HMD's, e.g., HTC
%   Vive, Valve Index, Oculus Rift. Follow setup instructions after
%   installing SteamVR. Use of Oculus devices needs MonadoXR as a SteamVR
%   driver plugin on Linux (see setup instructions under
%   https://monado.freedesktop.org/steamvr.html). When displaying in
%   'Monoscopic' or 'Stereoscopic' 2D mode, it has been shown beneficial at
%   least on Linux with Oculus Rift, to disable asynchronous reprojection,
%   as this reduces jitter and tracking noise. HTC Vive devices do not need
%   extra external Monado plugins for use with SteamVR.
%
% macOS:
% ------
%
% No OpenXR (or other Psychtoolbox supported) virtual reality runtime
% exists on Apples iToys operating system.
%
