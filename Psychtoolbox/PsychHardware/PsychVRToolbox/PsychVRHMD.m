function varargout = PsychVRHMD(cmd, varargin)
% PsychVRHMD() - High level driver for VR HMD devices.
%
% This driver bundles all the common high level functionality
% of different Virtual Reality Head mounted displays into one
% function.
%
% It dispatches generic calls into appropriate device specific
% drivers as needed.
%
% Usage:
%
% hmd = PsychVRHMD('AutoSetupHMD' [, basicTask][, basicRequirements][, basicQuality][, vendor][, deviceIndex]);
% - Automatically detect the first connected HMD, set it up with reasonable
% default parameters, and return a device handle 'hmd' to it. If the system
% does not support any HMDs, not even emulated ones, just does nothing and
% returns an empty handle, ie., hmd = [], so caller can cope with that.
%
% Optional parameters: 'basicTask' what kind of task should be implemented.
% The default is 'Tracked3DVR', which means to setup for stereoscopic 3D
% rendering, driven by head motion tracking, for a fully immersive experience
% in some kind of 3D virtual world. This is the default if omitted. '3DVR' sets
% up for stereoscopic 3D rendering that is not driven by head motion tracking.
%
% The task 'Stereoscopic' sets up for display of stereoscopic stimuli, but without
% head tracking. 'Monoscopic' sets up for display of monocular stimuli, ie.
% the HMD is just used as a special kind of standard display monitor.
%
% In monoscopic or stereoscopic mode, you can change the imaging parameters, ie.,
% apparent size and location of the 2D views used with the following command to
% optimize visual display:
%
% PsychVRHMD('View2DParameters', hmd, eye [, position][, size][, orientation]);
% The command is fully supported under the OpenXR driver, but does nothing
% and only returns NaN results on other drivers like the old Oculus,
% Oculus-1 and OpenHMD drivers.
%
%
% 'basicRequirements' defines basic requirements for the task. Currently
% defined are the following strings which can be combined into a single
% 'basicRequirements' string:
%
%
% 'DebugDisplay' = Show the output which is displayed on the HMD inside the
% Psychtoolbox onscreen window as well. This will have a negative impact on
% performance, latency and timing of the HMD visual presentation, so should only
% be used for debugging, as it may cause a seriously degraded VR experience.
% By default, no such debug output is produced and the Psychtoolbox onscreen
% window is not actually displayed on the desktop. This option is silently ignored
% on the old classic Oculus driver at the moment.
%
% 'Float16Display' = Request rendering, compositing and display in 16 bpc float
% format on some HMD's and drivers. This will ask Psychtoolbox to render and
% post-process stimuli in 16 bpc linear floating point format, and allocate 16 bpc
% half-float textures as final renderbuffers to be sent to the VR compositor.
% If the VR compositor takes advantage of the high source image precision is at
% the discretion of the compositor and HMD. By default, if this request is omitted,
% processing and display in sRGB format is requested from Psychtoolbox and the compositor
% on some drivers, e.g., for the Oculus 1.11+ runtime and Rift CV1, ie., a roughly
% gamma 2.2 8 bpc format is used, which is optimized for the gamma response curve of
% at least the Oculus Rift CV1 display. On other HMDs or drivers, this option may do
% nothing and get silently ignored.
%
% 'ForceSize=widthxheight' = Enforce a specific fixed size of the stimulus
% image buffer in pixels, overriding the recommmended value by the runtime,
% e.g., 'ForceSize=2200x1200' for a 2200 pixels wide and 1200 pixels high
% image buffer. By default the driver will choose values that provide good
% quality for the given VR/AR/MR/XR display device, which can be scaled up
% or down with the optional 'pixelsPerDisplay' parameter for a different
% quality vs. performance tradeoff in the function PsychVRHMD('SetupRenderingParameters');
% The specified values are clamped against the maximum values supported by
% the given hardware + driver combination.
%
% 'ForbidMultiThreading' = Forbid any use of multi-threading for visual
% presentation by the driver for any means or purposes! This is meant to
% get your setup going in case of severe bugs in proprietary OpenXR
% runtimes that can cause instability, hangs, crashes or other malfunctions
% when multi-threading is used. Or if one wants to squeeze out every last
% bit of performance, no matter the consequences ("Fast and furious mode").
% On many proprietary OpenXR runtimes, this will prevent any reliable,
% trustworthy, robust or accurate presentation timing or timestamping, and
% may cause severe visual glitches under some modes of operation. See the
% following keywords below for descriptions of various more nuanced
% approaches to multi-threading vs. single-threading to choose fine-tuned
% tradeoffs between performance, stability and correctness for your
% specific experimental needs.
%
% 'Use2DViewsWhen3DStopped' = Ask the driver to switch to use of the same 2D views
% and geometry during the '3DVR' or 'Tracked3DVR' basicTask as would be used
% for pure 2D display in basicTask 'Stereoscopic' whenever the user script
% signals it does not execute a tight tracking and animation loop, ie.
% whenever the script calls PsychVRHMD('Stop', hmd). Switch back to regular
% 3D projected geometry and views after a consecutive PsychVRHMD('Start', hmd).
% This is useful if have phases in your experiment session when you want to
% display non-tracked content, e.g., instructions or feedback to the
% subject between trials, fixation crosses, etc., or pause script execution
% for more than a few milliseconds, but still want the visual display to
% stay stable. If this keyword is omitted, depending on the specific OpenXR
% runtime in use, the driver will stabilize the regular 3D projected
% display by use of multi-threaded operation when calling PsychVRHMD('Stop', hmd),
% and resume single-threaded operation after PsychVRHMD('Start', hmd). This
% higher overhead mode of operation via multi-threading will possibly have
% degraded performance, and not only between the 'Stop' and 'Start' calls,
% but throughout the whole session! This is why it can be advisable to
% evaulate if use of the 'Use2DViewsWhen3DStopped' keyword is a better
% solution for your specific experiment paradigm. The switching between 3D
% projected view and standard 2D stereoscopic view will change the image
% though, which may disorient the subject for a moment while the subjects
% eyes need to adapt their accomodation, vergence and focus point. You can
% change the imaging parameters, ie., apparent size and location of the 2D
% views used in this mode with the following command to minimize visual
% disorientation:
%
% PsychVRHMD('View2DParameters', hmd, eye [, position][, size][, orientation]);
%
% For such 2D views you can also specify the distance of the virtual
% viewscreen in meters in front of the eyes of the subject. By default the
% distance is 1 meter and the size and position is set up to fill out the
% field of view in a meaningful way, essentially covering the whole
% available field of view. By overriding the distance to a smaller or
% bigger distance than 1 meter, you can "zoom in" to the image, or make
% sure that also the corners and edges of the image are visible. E.g., the
% following keyword would place the virtual screen at 2.1 meters distance:
%
% '2DViewDistMeters=2.1'
%
% 'DontCareAboutVisualGlitchesWhenStopped' = Tell the driver that you don't
% care about potential significant visual presentation glitches happening if
% your script does not run a continuous animation with high framerate, e.g.,
% after calling PsychVRHMD('Stop', hmd), pausing, etc. This makes sense if
% you don't care, or if your script does not ever pause or slow down during
% a session or at least an ongoing trial. This will avoid multi-threading
% for glitch prevention in such cases, possibly allowing to side-step
% certain bugs in proprietary OpenXR runtimes, or to squeeze out higher
% steady-state performance.
%
% 'TimingPrecisionIsCritical' = Signal that visual presentation timing and
% timestamping of visual stimuli should be given highest importance -
% essentially above all else. You still need to specify the following
% keywords relating to the specifics of your timing/timestamping needs, but
% this specific requirement is a signal to make all tradeoffs, including
% choice of drivers to use, almost solely based on their timing properties.
%
% 'NoTimingSupport' = Signal no need at all for high precision and reliability
% timing for presentation. If you don't need any timing precision or
% reliability in your script, specifying this keyword may allow the driver
% to optimize for higher performance. See 'TimingSupport' explanation right
% below:
%
% 'TimingSupport' = Use high precision and reliability timing for presentation.
% Please note that generally only the special Linux VR/AR/MR/XR drivers are
% currently capable of robust, reliable, trustworthy and accurate timing,
% and sometimes even they need special configuration or have some caveats,
% specifically:
%
% - The original PsychOculusVR driver has perfect timing, but only works on
%   Linux/X11 with a separate X-Screen for the HMD, and only works with the
%   original Oculus Rift DK1 and DK2 VR HMDs.
%
% - The PsychOpenHMDVR driver has perfect timing, but only works on
%   Linux/X11 with a separate X-Screen for the HMD, and only works with the
%   subset of VR HMDs supported by OpenHMD, and often various caveats
%   apply for those HMDs, like imperfect optical undistortion, or lack of
%   full 6 DoF head tracking - Often only 3 DoF orientation tracking is
%   supported.
%
% - The PsychOculusVR1 driver for the Oculus VR 1.x runtime on MS-Windows
%   has essentially unreliable/not trustworthy timing and timestamping.
%
% - The timing properties of the PsychOpenXR driver are highly dependent on
%   the OpenXR runtime at use. Citing from the 'help PsychOpenXR':
%
%     The current OpenXR specification, as of OpenXR version v1.0.26 from January 2023,
%     does not provide any means of reliable, trustworthy, accurate timestamping of
%     presentation, and all so far tested proprietary OpenXR runtime implementations
%     have severely broken and defective timing support. Only the open-source
%     Monado OpenXR runtime on Linux provides a reliable and accurate timing
%     implementation. Therefore this driver has to use a workaround on non-Monado
%     OpenXR runtimes to achieve at least ok'ish timing if you require it, and
%     that workaround involves multi-threaded operation. This multi-threading
%     in turn can severely degrade performance, possibly reducing achievable
%     presentation framerates to (less than) half of the maximum video refresh
%     rate of your HMD! For this reason you should only request 'TimingSupport'
%     on non-Monado if you really need it and be willing to pay the performance
%     price.
%
%     If you omit this keyword, the driver will try to guess if you need
%     precise presentation timing for your session or not. As long as you only
%     call Screen('Flip', window) or Screen('Flip', window, [], ...), ie. don't
%     specify a requested stimulus onset time, the driver assumes you don't
%     need precise timing, just presenting as soon as possible after a
%     Screen('Flip'), and also that you don't care about accurate or trustworthy
%     or correct presentation timestamps to be returned by Screen('Flip'). Once
%     you specify a target onset time tWhen, ie. via calling 'Flip' as
%     Screen('Flip', window, tWhen [, ...]), the driver assumes from then on
%     and for the rest of the session that you want reasonably accurate
%     presentation timing. It will then switch to multi-threaded operation with
%     better timing, but potentially drastically reduced performance.
%
% 'TimestampingSupport' = Use high precision and reliability timestamping for presentation.
% 'NoTimestampingSupport' = Do not need high precision and reliability timestamping for presentation.
% Those keywords let you specify if you definitely need or don't need
% trustworthy, reliable, robust, precise presentation timestamps, ie. the
% 'timestamp' return values of timestamp = Screen('Flip') should be high
% quality, or if you don't care. If you omit both keywords, the driver will
% try to guess what you wanted. On most current OpenXR runtimes, use of
% timestamping will imply multi-threaded operation with the performance
% impacts and problems mentioned above in the section about 'TimingSupport',
% that is why it is advisable to explicitely state your needs, to allow the
% driver to optimize for the best precision/reliability/performance
% tradeoff on all the runtimes where such a tradeoff is required.
% Notable exceptions are the Linux PsychOculus and PsychOpenHMDVR
% drivers when used on separate X-Screens for their HMDs, and some
% configurations of the Monado OpenXR runtime on Linux, where
% timestamps are trustworthy without performance tradeoffs or other
% known problems. The PsychOculusVR1 driver on MS-Windows always
% provides untrustworthy timestamps, no matter what.
%
% 'TimeWarp' = Enable per eye image 2D timewarping via prediction of eye
% poses at scanout time. This mostly only makes sense for head-tracked 3D
% rendering. Depending on 'basicQuality' a more cheap or more expensive
% procedure is used. On the v1.11 Oculus runtime and Rift CV1, 'TimeWarp'
% is always active, so this option is redundant.
%
% 'LowPersistence' = Try to keep exposure time of visual images on the retina
% low if possible, ie., try to approximate a pulse-type display instead of a
% hold-type display if possible. On the Oculus Rift DK2 with the original Oculus
% runtime on Linux, it will enable low persistence scanning of the OLED
% display panel, to light up each pixel only a fraction of a video refresh
% cycle duration. On any other HMD hardware or runtime this setting does
% not have any effect and is thereby pretty much redundant.
%
% 'PerEyeFOV' = Request use of per eye individual and asymmetric fields of view, even
% when the 'basicTask' was selected to be 'Monoscopic' or 'Stereoscopic'. This allows
% for wider field of view in these tasks, but requires the usercode to adapt to these
% different and asymmetric fields of view for each eye, e.g., by selecting proper 3D
% projection matrices for each eye. If a 'basicTask' of '3DVR' for non-tracked 3D, or
% (the default) 'Tracked3DVR' for head tracking driven 3D is selected, then that implies
% per-eye individual and asymmetric fields of view, iow. 'PerEyeFOV' is implied. For pure
% 'basicTask' of 'Monoscopic' or 'Stereoscopic' for Screen() 2D drawing, the system uses
% identical and symmetric fields of view for both eyes by default, so 'PerEyeFOV' would
% be needed to override this choice. COMPATIBILITy NOTE: Psychtoolbox-3 releases before
% June 2017 always used identical and symmetric fields of view for both eyes, which was
% a bug. However the error made was very small, due to the imaging properties of the
% Oculus Rift DK2, essentially imperceptible to the unknowing observer with the naked
% eye. Releases starting June 2017 now use separate fields of view in 3D rendering
% modes, and optionally for 2D mono/stereo modes with this 'PerEyeFOV' opt-in parameter,
% so stimulus display may change slightly for the same HMD hardware and user-code,
% compared to older Psychtoolbox-3 releases. This change was crucial to accomodate the
% rather different imaging properties of the Oculus Rift CV1 and possible other future
% HMD's. Note: This requirement is currently ignored with the standard OpenXR backend,
% as the OpenXR runtimes decide by themselves what is best here.
%
% 'FastResponse' = Try to switch images with minimal delay and fast
% pixel switching time. This will enable OLED panel overdrive processing
% on the Oculus Rift DK1 and DK2. OLED panel overdrive processing is a
% relatively expensive post processing step. On any other VR device and
% runtime other than Oculus Rift DK1/DK2 this option currently has no
% effect and is therefore redundant.
%
%
% 'Eyetracking' = Request eye gaze tracking via a supported HMD builtin eye tracker.
% This keyword asks the driver to enable eye gaze tracking. A given combo
% of VR/AR/MR device (and its builtin eye tracker), operating system, OpenXR
% runtime and additional optionally installed eye tracking software, may
% support multiple different gaze tracking api's and runtimes. By default,
% the driver will try to use the most capable gaze tracking api, ie. the
% one which provides the most detailed and exhaustive information about the
% users gaze, at the highest sampling rate, with the most flexibility. It
% will fall back to less capable or efficient tracking api's if more
% capable ones are not supported or available. For this reason, the amount
% of information can differ widely between the most capable api's and the
% most basic api's. User scripts which strive to be usable on different
% operating systems, software setups or eyetracking hardware and HMDs must
% therefore be written in a defensive and adaptive way to be able to work
% with only the minimal subset of information guaranteed to be available on
% all implementations. The info struct returned by info = PsychVRHMD('GetInfo');
% contains info about basic gaze tracking capabilities as a bitmask in
% info.eyeTrackingSupported: A value of +1 means at least one gaze vector
% is reported. A value of +2 means reporting of binocular eye tracking data
% is supported. A value of +1024 means that HTC's proprietary SRAnipal
% eyetracking is used for more extensive gaze data reporting.
%
% If eye tracking is requested via the keyword and supported, then the user
% script can request return of the most recent eye gaze tracking sample
% data by calling the state = PsychVRHMD('PrepareRender', ..., reqmask, ...)
% function with reqmask flag +4. This will cause the returned 'state' struct
% to contain additional fields with information about the most recent gaze.
% See help text for the 'PrepareRender' function for more detailed info.
%
% The current driver supports the following gazetracking implementations:
%
% - With HTC VR HMDs with eyetracking support, under Microsoft Windows, in
% combination with the optional HTC SRAnipal runtime DLL's installed, and
% using Matlab, an optional SRAnipal mex driver can be used to provide both
% binocular per-eye gaze tracking data, separate for the subjects left and
% right eye, and a virtual 3rd "cyclops eye" which is synthesized info from
% both hardware eye trackers, sometimes of higher quality due to sensor
% fusion of the two gaze tracker data streams. For each of both eyes, in
% addition to eye gaze position and direction, estimated pupil size in
% millimeters and a measure of eye openess is reported, e.g., for eye
% blink detection or estimation of gaze data reliability. Reported times
% are hardware timestamps of when a gaze sample was measured. On the tested
% "HTC Vive Pro Eye" HMD sampling rates of up to 120 Hz were possible.
%
% - On other device + operating system + OpenXR runtime combos with OpenXR
% gazetracking support, information from the XR_EXT_eye_gaze_interaction
% gaze tracking extension is returned. This extension is supported on a wider
% range of XR devices, but the returned information is more limited: A
% single eye gaze vector and position, but without any information about the
% subjects eye openess, pupil size or of the systems confidence in the quality
% of the measured gaze. The gaze vector is of unspecified origin. It could
% be measured gaze from a monocular eye tracker, ie. either left or right eye
% gaze, or it could be a "cyclops eye" synthesized gaze computed via sensor
% fusion of gaze data from a binocular gaze tracker. The gaze data may be
% measured data from a time in the past, or interpolated or extrapolated
% gaze data from one or more past measured eye gaze samples. The returned
% gaze sample timestamp may be a hardware timestamp of when the gaze sample
% was measured, but could also be the time for which gaze was predicted via
% interpolation or extrapolation of past hardware measured gaze samples.
% Temporal resolution of the gaze data is also unspecified. On the tested
% HTC Vive Pro Eye, the reported gaze seems to correspond to the sensor
% fusion of gaze samples from the binocular eye tracker, and the temporal
% resolution is reduced to at best 16.6 msecs for at most 60 gaze samples
% per second.
%
%
% These basic requirements get translated into a device specific set of
% settings. The settings can also be specific to the selected 'basicTask',
% and if a quality vs. performance / system load tradeoff is unavoidable
% then the 'basicQuality' parameter may modulate the strategy.
%
% 'basicQuality' defines the basic tradeoff between quality and required
% computational power. A setting of 0 gives lowest quality, but with the
% lowest performance requirements. A setting of 1 gives maximum quality at
% maximum computational load. Values between 0 and 1 change the quality to
% performance tradeoff.
%
% By default all currently supported types of HMDs from different
% vendors are probed and the first one found is used. If the optional
% parameter 'vendor' is provided, only devices from that vendor are
% detected and the first detected device is chosen.
%
% If additionally the optional 'deviceIndex' parameter is provided then
% that specific device 'deviceIndex' from that 'vendor' is opened and set up.
%
%
% PsychVRHMD('SetAutoClose', hmd, mode);
% - Set autoclose mode for HMD with handle 'hmd'. 'mode' can be
% 0 (this is the default) to not do anything special. 1 will close
% the HMD 'hmd' when the onscreen window is closed which displays
% on the HMD. 2 will do the same as 1, but close all open HMDs and
% shutdown the complete driver and runtime - a full cleanup.
%
%
% isOpen = PsychVRHMD('IsOpen', hmd);
% - Returns 1 if 'hmd' corresponds to an open HMD, 0 otherwise.
%
%
% PsychVRHMD('Close' [, hmd])
% - Close provided HMD device 'hmd'. If no 'hmd' handle is provided,
% all HMDs will be closed and the driver will be shutdown.
%
%
% PsychVRHMD('Controllers', hmd);
% - Return a bitmask of all connected controllers: Can be the bitand
% of the OVR.ControllerType_XXX flags described in 'GetInputState'.
% This does not detect if controllers are hot-plugged or unplugged after
% the HMD was opened. Iow. only probed at 'Open'.
%
%
% info = PsychVRHMD('GetInfo', hmd);
% - Retrieve a struct 'info' with information about the HMD 'hmd'.
% The returned info struct contains at least the following standardized
% fields with information:
% handle = Driver internal handle for the specific HMD.
% driver = Function handle to the actual driver for the HMD, e.g., @PsychOculusVR.
% type   = Defines the type/vendor of the device, e.g., 'Oculus'.
% subtype = Defines the type of driver more specific, e.g., 'Oculus-classic' or 'Oculus-1'.
% modelName = Name string with the name of the model of the device, e.g., 'Rift DK2'.
%
% separateEyePosesSupported = 1 if use of PsychVRHMD('GetEyePose') will improve
%                             the quality of the VR experience, 0 if no improvement
%                             is to be expected, so 'GetEyePose' can be avoided
%                             to save processing time without a loss of quality.
%                             This will be zero (== no benefit) for all modern runtimes.
%
% VRControllersSupported = 1 if use of PsychVRHMD('GetInputState') will provide input
%                            from actual dedicated VR controllers. Value is 0 if
%                            controllers are only emulated to some limited degree,
%                            e.g., by abusing a regular keyboard as a button controller,
%                            ie. mapping keyboard keys to OVR.Button_XXX buttons.
%
% handTrackingSupported = 1 if PsychVRHMD('PrepareRender') with reqmask +2 will provide
%                           valid hand tracking info, 0 if this is not supported and will
%                           just report fake values. A driver may report 1 here but still
%                           don't provide meaningful info at runtime, e.g., if required
%                           tracking hardware is missing or gets disconnected. The flag
%                           just aids extra performance optimizations in your code.
%
% hapticFeedbackSupported = 1 if basic haptic feedback is supported in principle on some controllers.
%                             0 otherwise. A flag of zero means no haptic feedback support, but
%                             a flag of 1 may still mean no actual feedback, e.g., if suitable
%                             hardware is not configured and present. Flags higher than 1 can
%                             signal presence of more advanced haptic feedback, so you should
%                             test for a setting == 1 to know if PsychVRHMD('HapticPulse') works
%                             in principle, which is considered basic feedback ability.
%
% eyeTrackingSupported = Info about eye gaze tracking capabilities of the given VR/AR/MR device and
%                        software setup. A value of 0 means that eye gaze tracking is not supported.
%                        A value of +1 means at least one gaze vector is reported. A value of +2 means
%                        reporting of binocular per-eye tracking data is supported. A value of
%                        +1024 means that HTC's proprietary SRAnipal eyetracking is available for
%                        more extensive gaze data reporting.
%
%
% The info struct may contain much more vendor specific information, but the above
% set is supported across all devices.
%
%
% [isVisible, playAreaBounds, OuterAreaBounds] = PsychVRHMD('VRAreaBoundary', hmd [, requestVisible]);
% - Request visualization of the VR play area boundary for 'hmd' and returns its
% current extents.
%
% 'requestVisible' 1 = Request showing the boundary area markers, 0 = Don't
% request showing the markers.
% If the driver can control area boundary visibility is highly dependent on the VR
% driver in use. This flag gets ignored by most drivers. See driver specific help, e.g.,
% "help PsychOculusVR1", for behaviour of a specific driver.
%
% Some drivers or hardware setups may not support VR area boundaries at all, in
% which case the function will return empty boundaries.
%
% Returns in 'isVisible' the current visibility status of the VR area boundaries.
%
% 'playAreaBounds' is a 3-by-n matrix defining the play area boundaries. Each
% column represents the [x;y;z] coordinates of one 3D definition point. Connecting
% successive points by line segments defines the boundary, as projected onto the
% floor. Points are listed in clock-wise direction. An empty return argument means
% that the play area is so far undefined.
%
% 'OuterAreaBounds' defines the outer area boundaries in the same way as
% 'playAreaBounds'.
%
%
% input = PsychVRHMD('GetInputState', hmd, controllerType);
% - Get input state of controller 'controllerType' associated with HMD 'hmd'.
%
% Note that if the underlying driver does not support special VR controllers, ie.,
% hmdinfo = PsychVRHMD('GetInfo') returns hmdinfo.VRControllersSupported == 0, then
% only a minimally useful 'input' state is returned, which is based on emulating or
% faking input from real controllers, so this function will be of limited use. Specifically,
% on emulated controllers, only the input.Valid, input.Time and input.Buttons
% fields are returned, all other fields are missing.
%
% 'controllerType' can be one of OVR.ControllerType_LTouch, OVR.ControllerType_RTouch,
% OVR.ControllerType_Touch, OVR.ControllerType_Remote, OVR.ControllerType_XBox, or
% OVR.ControllerType_Active for selecting whatever controller is currently active.
%
% Return argument 'input' is a struct with fields describing the state of buttons and
% other input elements of the specified 'controllerType'. It has the following fields:
%
% 'Valid' = 1 if 'input' contains valid results, 0 if input status is invalid/unavailable.
% 'ActiveInputs' = Bitmask defining which of the following struct elements do contain
% meaningful input from actual physical input source devices. This is a more fine-grained
% reporting of what 'Valid' conveys, split up into categories. The following flags will be
% logical or'ed together if the corresponding input category is valid, ie. provided with
% actual input data from some physical input source element, controller etc.:
%
% +1  = 'Buttons' gets input from some real buttons or switches.
% +2  = 'Touches' gets input from some real touch/proximity sensors or gesture recognizers.
% +4  = 'Trigger' gets input from some real analog trigger sensor or gesture recognizer.
% +8  = 'Grip' gets input from some real analog grip sensor or gesture recognizer.
% +16 = 'Thumbstick' gets input from some real thumbstick, joystick or trackpad or similar 2D sensor.
% +32 = 'Thumbstick2' gets input from some real secondary thumbstick, joystick or trackpad or similar 2D sensor.
%
% 'Time' Time of last input state change of controller.
% 'Buttons' Vector with button state on the controller, similar to the 'keyCode'
% vector returned by KbCheck() for regular keyboards. Each position in the vector
% reports pressed (1) or released (0) state of a specific button. Use the OVR.Button_XXX
% constants to map buttons to positions.
%
% 'Touches' Like 'Buttons' but for touch buttons. Use the OVR.Touch_XXX constants to map
% touch points to positions.
%
% 'Trigger'(1/2) = Left (1) and Right (2) trigger: Value range 0.0 - 1.0, filtered and with dead-zone.
% 'TriggerNoDeadzone'(1/2) = Left (1) and Right (2) trigger: Value range 0.0 - 1.0, filtered.
% 'TriggerRaw'(1/2) = Left (1) and Right (2) trigger: Value range 0.0 - 1.0, raw values unfiltered.
% 'Grip'(1/2) = Left (1) and Right (2) grip button: Value range 0.0 - 1.0, filtered and with dead-zone.
% 'GripNoDeadzone'(1/2) = Left (1) and Right (2) grip button: Value range 0.0 - 1.0, filtered.
% 'GripRaw'(1/2) = Left (1) and Right (2) grip button: Value range 0.0 - 1.0, raw values unfiltered.
%
% 'Thumbstick' = 2x2 matrix: Column 1 contains left thumbsticks [x;y] axis values, column 2 contains
%  right sticks [x;y] axis values. Values are in range -1 to +1, filtered and with deadzone applied.
% 'ThumbstickNoDeadzone' = Like 'Thumbstick', filtered, but without a deadzone applied.
% 'ThumbstickRaw' = 'Thumbstick' raw date without deadzone or filtering applied.
%
% Some devices driven by an OpenXR runtime may also expose a 'Thumbstick2' field, with same semantic
% as the 'Thumbstick' 2x2 matrix, but for secondary 2D input sources, e.g., a 2nd thumbstick,
% joystick or trackpad or similar for each hand-controller. The presence of the 'Thumbstick2' field
% in the 'input' struct is not guaranteed, unless 'ActiveInputs' contains the +32 flag 'Thumbstick2'.
%
% pulseEndTime = PsychVRHMD('HapticPulse', hmd, controllerType [, duration=XX][, freq=1.0][, amplitude=1.0]);
% - Trigger a haptic feedback pulse, some controller vibration, on the specified 'controllerType'
% associated with the specified 'hmd'. 'duration' is desired pulse duration in seconds. On Oculus
% devices, by default a maximum of 2.5 seconds pulse is executed, but other vendors devices may have
% a different maximum. 'freq' is normalized frequency in range 0.0 - 1.0. A value of 0 will try to
% disable an ongoing pulse. How this normalized 'freq' maps to a specific haptic device is highly
% device and runtime dependent.
%
% 'amplitude' is the amplitude of the vibration in normalized 0.0 - 1.0 range.
%
% 'pulseEndTime' returns the expected stop time of vibration in seconds, given the parameters.
% Currently the function will return immediately for a (default) 'duration', and the pulse
% will end after the maximum duration supported by the given device. Smaller 'duration' values than
% the maximum duration will block the execution of the function until the 'duration' has passed on
% some types of controllers.
%
% Please note that behaviour of this function is highly dependent on the type of VR driver and
% devices used. You should consult driver specific documentation for details, e.g., the help of
% 'PsychOculusVR' or 'PsychOculusVR1' for Oculus systems. On some drivers the function may do
% nothing at all, e.g., if the 'GetInfo' function returns info.hapticFeedbackSupported == 0.
%
%
% state = PsychVRHMD('PrepareRender', hmd [, userTransformMatrix][, reqmask=1][, targetTime]);
% - Mark the start of the rendering cycle for a new 3D rendered stereoframe.
% Return a struct 'state' which contains various useful bits of information
% for 3D stereoscopic rendering of a scene, based on head tracking data.
%
% 'hmd' is the handle of the HMD which delivers tracking data and receives the
% rendered content for display.
%
% 'reqmask' defines what kind of information is requested to be returned in
% struct 'state'. Only query information you actually need, as computing some
% of this info is expensive! See below for supported values for 'reqmask'.
%
% 'targetTime' is the expected time at which the rendered frame will display.
% This could potentially be used by the driver to make better predictions of
% camera/eye/head pose for the image. Omitting the value will use a target time
% that is implementation specific, but known to give generally good results,
% e.g., the midpoint of scanout of the next video frame.
%
% 'userTransformMatrix' is an optional 4x4 right hand side (RHS) transformation
% matrix. It gets applied to the tracked head pose as a global transformation
% before computing results based on head pose like, e.g., camera transformations.
% You can use this to translate the "virtual head" and thereby the virtual eyes/
% cameras in the 3D scene, so observer motion is not restricted to the real world
% tracking volume of your headset. A typical 'userTransformMatrix' would be a
% combined translation and rotation matrix to position the observer at some
% 3D location in space, then define his/her global looking direction, aka as
% heading angle, yaw orientation, or rotation around the y-axis in 3D space.
% Head pose tracking results would then operate relative to this global transform.
% If 'userTransformMatrix' is left out, it will default to an identity transform,
% in other words, it will do nothing.
%
%
% state always contains a field state.tracked, whose bits signal the status
% of head tracking for this frame. A +1 flag means that head orientation is
% tracked. A +2 flag means that head position is tracked via some absolute
% position tracker like, e.g., the Oculus Rift DK2 or Rift CV1 camera. A +128
% flag means the HMD is actually strapped onto the subjects head and displaying
% our visual content. Lack of this flag means the HMD is off and thereby blanked
% and dark, or we lost access to it to another application.
%
% state also always contains a field state.SessionState, whose bits signal general
% VR session status:
%
% +1  = Our rendering goes to the HMD, ie. we have control over it. Lack of this could
%       mean the Health and Safety warning is displaying at the moment and waiting for
%       acknowledgement, or the Oculus GUI application is in control.
% +2  = HMD is present and active.
% +4  = HMD is strapped onto users head. E.g., a Oculus Rift CV1 would switch off/blank
%       if not on the head.
% +8  = DisplayLost condition! Some hardware/software malfunction, need to completely quit this
%       Psychtoolbox session to recover from this.
% +16 = ShouldQuit The user interface / user asks us to voluntarily terminate this session.
% +32 = ShouldRecenter = The user interface asks us to recenter/recalibrate our tracking origin.
%
% 'reqmask' defaults to 1 and can have the following values added together:
%
% +1 = Return matrices for left and right "eye cameras" which can be directly
%      used as OpenGL GL_MODELVIEW matrices for rendering the scene. 4x4 matrices
%      for left- and right eye are contained in state.modelView{1} and {2}.
%
%      Return position and orientation 4x4 camera view matrices which describe
%      position and orientation of the "eye cameras" relative to the world
%      reference frame. They are the inverses of state.modelView{}. These
%      matrices can be directly used to define cameras for rendering of complex
%      3D scenes with the Horde3D 3D engine. Left- and right eye matrices are
%      contained in state.cameraView{1} and state.cameraView{2}.
%
%      Additionally tracked/predicted head pose is returned in state.localHeadPoseMatrix
%      and the global head pose after application of the 'userTransformMatrix' is
%      returned in state.globalHeadPoseMatrix - this is the basis for computing
%      the camera transformation matrices.
%
% +2 = Return matrices for tracked left and right hands of user, ie. of tracked positions
%      and orientations of left and right hand tracking controllers, if any. See also
%      section about 'GetInfo' for some performance comments.
%
%      state.handStatus(1) = Tracking status of left hand: 0 = Untracked, 1 = Orientation
%                            tracked, 2 = Position tracked, 3 = Orientation and position
%                            tracked. If handStatus is == 0 then all the following information
%                            is invalid and can not be used in any meaningful way.
%
%      state.handStatus(2) = Tracking status of right hand.
%
%      state.localHandPoseMatrix{1} = 4x4 OpenGL right handed reference frame matrix with
%                                     hand position and orientation encoded to define a
%                                     proper GL_MODELVIEW transform for rendering stuff
%                                     "into"/"relative to" the oriented left hand.
%
%      state.localHandPoseMatrix{2} = Ditto for the right hand.
%
%      state.globalHandPoseMatrix{1} = userTransformMatrix * state.localHandPoseMatrix{1};
%                                      Left hand pose transformed by passed in userTransformMatrix.
%
%      state.globalHandPoseMatrix{2} = Ditto for the right hand.
%
%      state.globalHandPoseInverseMatrix{1} = Inverse of globalHandPoseMatrix{1} for collision
%                                             testing/grasping of virtual objects relative to
%                                             hand pose of left hand.
%
%      state.globalHandPoseInverseMatrix{2} = Ditto for right hand.
%
% +4 = Return the most recent eye gaze information on devices with built-in eye tracking hardware.
%      Returned information may represent the latest available measured eye
%      gaze data, or it may be predicted eye gaze information for the
%      specified 'targetTime', computed via interpolation or extrapolation
%      from actual previously measured eye gaze. This is dependent on the
%      specific gaze tracker implementation of your system. If the reported
%      gaze sample timestamps are identical to the provided 'targetTime'
%      then that is one possible indication that reported gaze may be
%      predicted gaze instead of a direct hardware measured gaze sample.
%
%      The following fields are mandatory as part of the state struct, if gaze
%      tracking is supported and enabled and requested:
%
%      state.gazeRaw = If no new gaze tracking data is available, returns an
%      empty [] variable. Otherwise a variable in a format that is
%      dependent on the actually used gaze tracking api and implementation.
%      It could be a vector, a struct, an array of structs... The format
%      may change without prior notice, without any regard for backward
%      compatibility, so it is mostly useful for debugging by the PTB
%      developers or other Psychtoolbox internal special use cases, not to
%      be relied on by regular user experiment scripts!
%
%      The following variables are arrays, whose length depends on the
%      used gaze tracking method. Each array element represents properties
%      of one tracked eye gaze. At a minimum, the arrays have one element
%      for the most basic gaze tracking, e.g., if the OpenXR extension
%      XR_EXT_eye_gaze_interaction is used for gaze tracking, it will only
%      report one gaze vector in index 1: A monocular gaze sample from either
%      the left or right eye, or a synthetic "cyclops eye" gaze sample, computed
%      via sensor fusion of data from a binocular gazetracker. The arrays could
%      also have 2 elements for a purely binocular eye tracker, with index 1 for
%      the left eye, and index 2 for the right eye data. On a binocular tracker,
%      it is also possible for a three element array to be returned, in
%      which case index 1 is left eye date, 2 is right eye date, and 3 is
%      synthesized "cyclops eye" data.
%
%      Please write your scripts so they can handle any number of 1, 2 or
%      three array elements meaningfully:
%
%      state.gazeStatus(i) = A flag telling if i'th gaze is unavailable
%                            (=0), available (+1) or available and somewhat
%                            trustworthy (+2). Values other than 3 (=1+2)
%                            should not really be trusted. A value of only
%                            1 could, e.g., mean that data was reported,
%                            but it is not based on an actual measured eye
%                            gaze sample, but purely extrapolated or
%                            predicted from past valid data. A value of 3
%                            is not a guarantee of high quality data, just
%                            that the data is actually measured eye gaze
%                            data and passed the minimum quality treshold.
%
%
%      state.gazeTime(i) = A timestamp of the time for which the given
%      gaze information is valid, or the value NaN if no valid timestamp is
%      available from the gaze tracker. Depending on gaze tracking method in
%      use, this could be a time in the past, referring to the hardware
%      timestamp of when the gaze tracker hardware acquired that sample, or
%      it could be the time in the past or near future for which the gaze
%      data was computed via prediction / extrapolation of gaze movement or
%      interpolation from past gaze tracking data history. OpenXR built in
%      gaze tracking extensions often may not report the most recent
%      measured eye gaze sample from a past tracking cycle. Instead they
%      take the user provided 'targetTime' (or predicted stimulus onset
%      time for the next to-be-presented VR/AR/MR/XR stimulus image, if
%      'targetTime' was omitted) and try to predict where the subject will
%      be looking (for a 'targetTime' in the near future) or has looked
%      (for a 'targetTime' in the near past). In case of such prediction,
%      the reported state.gazeTime(i) corresponds to the time for which
%      gaze was actually predicted. It is a bit of a hazard for scientific
%      research purposes that there is some uncertainty if timestamps refer
%      to time of real measured gaze, or to some predicted time, or that
%      prediction / interpolation / extrapolation may be used instead of
%      reporting measured data, or that the prediction method - if any - is
%      not specified or standardized across different devices, gaze
%      trackers and gaze tracking runtimes and api's. This is unfortunately
%      unavoidable, as most commercial off the shelf gaze trackers for XR
%      applications are not targeted at scientific research use cases, but
%      as human computer interaction method for operating and navigating in
%      VR and AR, e.g., for gaming and entertainment purposes. Not much we
%      could do about this, so you will have to deal with this in your
%      research paradigm or carefully select hardware with known suitable
%      properties for your specific use case.
%
%      Actual gaze information is provided in two formats, a 2D format, in
%      onscreen window pixel coordinates, ie. where in the image has the
%      subject looked, and a 3D format, as 3D gaze rays, ie. where in a
%      rendered 3D scene has the subject looked:
%
%      2D - Onscreen window referenced:
%
%      state.gazePos{i} = A two-element [x,y] vector of the estimated 2D user
%      gaze position in Psychtoolbox onscreen window coordinates. Iow. the
%      x,y coordinates of where the user looked. In mono display mode this
%      is done by mapping the users gaze vector to the 2D space of the
%      common image that is displayed in the left and right eye display of
%      a VR/AR/MR HMD. In stereoscopic 2D display mode or full 3D perspective
%      correct rendering mode with potential head tracking, where a different
%      image is rendered and displayed to the subjects left and right eye, the
%      mapping of indices is as follows: state.gazePos{1} is expressed wrt.
%      to the left eye image buffer, ie. the one selected via
%      Screen('SelectStereoDrawBuffer', win, 0);. state.gazePos{2} refers
%      to the right eye image buffer (Screen('SelectStereoDrawBuffer', win, 0);).
%      state.gazePos{3} for a potential synthetic "cyclops eye" gaze will
%      reference the left eye image buffer again.
%
%      3D - 3D scene geometry referenced:
%
%      state.gazeRayLocal{i} = encodes the subjects gaze direction / line
%      of sight within a HMD fixed reference frame:
%
%      state.gazeRayLocal{i}.gazeC = a [x,y,z] 3D vector denoting the
%      estimated position of the optical center of the subjects eye balls,
%      relative to the origin of the head-fixed reference frame.
%
%      state.gazeRayLocal{i}.gazeD = a [dx,dy,dz] 3D vector denoting the
%      gaze direction in the head-fixed x, y and z axis.
%
%      The values in gazeRayLocal therefore define a 3D line equation
%      denoting the users line of sight, a "gaze ray" so to speak:
%
%      For all scalar values t from zero to infinity, p(t) with
%      p(t) = state.gazeRayLocal{i}.gazeC + t * state.gazeRayLocal{i}.gazeD
%      defines 3D points along the looking direction / gaze vector / gaze
%      ray of the subject, in a head-fixed reference frame.
%
%      Mathematical intersection of such a defined line equation p(t) with 3D
%      scene geometry in 3D rendering mode that is fixed wrt. to the users head
%      allows you to figure out where the user is looking in 3D space.
%
%      For a typical 3D head tracked VR / AR / MR rendering scenario, where
%      you would also set the the 'reqmask' flag +1 to retrieve head
%      tracking information and state.modelView matrices for 3D rendering,
%      the function also provides state.gazeRayGlobal{i} of the same
%      format. In this case the HMD head tracking information is used to
%      locate the subjects head position and orientation in a 3D rendered
%      scene and the gaze ray is transformed accordingly, so mathematical
%      intersection of rendered 3D geometry with the 3D line equation ...
%      p(t) = state.gazeRayGlobal{i}.gazeC + t * state.gazeRayGlobal{i}.gazeD
%      ... allows to find the point of fixation in a 3D world even if the
%      subject is moving their head or walking around.
%
%
%      Some of the supported eye tracking implementations may provide the
%      following additional optional information for each gaze index i. If
%      the information is not available for a given implementation, either
%      an empty vector [] or the scalar value NaN is returned:
%
%      state.gazeConfidence(i) = A scalar value of confidence, ie. how
%      certain is the gaze tracker that reported data is trustworthy and
%      accurate. Currently unsupported on all trackers, returns NaN.
%
%      state.gazeEyeOpening(i) = A scalar value of how far the subjects
%      eyes are open, in a normalized range 0 for closed to 1 for fully
%      open. This can be used, e.g., as another confidence measure, or for
%      eye blink detection. Supported for i=1,2 with HTC SRAnipal gaze
%      tracking on suitable HTC HMDs like the HTC Vive Pro Eye.
%
%      state.gazeEyePupilDiameter(i) = The estimated diameter of the
%      subjects pupil, presumably in millimeters. Supported for i=1,2 with
%      HTC SRAnipal gaze tracking on suitable HTC HMDs like the HTC Vive Pro Eye.
%
%      state.gazeEyeConvergenceDistance = For binocular gaze tracking, this
%      may be a scalar estimate of eye convergence distance, ie. the
%      distance of the fixation point from the eyes. May be supported on
%      some HTC HMDs under SRAnipal, but has not been confirmed to work in
%      practice on the tested HTC Vive Pro Eye.
%
%
% More flags to follow...
%
%
% eyePose = PsychVRHMD('GetEyePose', hmd, renderPass [, userTransformMatrix][, targetTime]);
% - Return a struct 'eyePose' which contains various useful bits of information
% for 3D stereoscopic rendering of the stereo view of one eye, based on head
% tracking data. This function provides essentially the same information as
% the 'PrepareRender' function, but only for one eye. Therefore you will need
% to call this function twice, once for each of the two renderpasses, at the
% beginning of each renderpass.
%
% Note: The 'GetEyePose' function is not implemented in a meaningful/beneficial
% way for modern supported types of HMD. This means that while the function will work
% on all supported HMDs, there will not be any benefit on most HMDs of using it in
% terms of performance or quality of the VR experience, because the underlying driver may
% only emulate / fake the results for compatibility. Currently only the original driver
% for the Oculus VR Rift DK1 and Rift DK2 supports this function in a way that could
% improve the VR experience, none of the other drivers does, not even the modern driver
% for recent Oculus HMDs. The info struct returned by PsychVRHMD('GetInfo') will return
% info.separateEyePosesSupported == 1 if there is a benefit to be expected from use
% of this function, or info.separateEyePosesSupported == 0 if no benefit is expected
% and simply using the data from PsychVRHMD('PrepareRender') will provide results with
% the same quality at a lower computational cost.
%
% 'hmd' is the handle of the HMD which delivers tracking data and receives the
% rendered content for display.
%
% 'renderPass' defines if information should be returned for the 1st renderpass
% (renderPass == 0) or for the 2nd renderpass (renderPass == 1). The driver will
% decide for you if the 1st renderpass should render the left eye and the 2nd
% pass the right eye, or if the 1st renderpass should render the right eye and
% then the 2nd renderpass the left eye. The ordering depends on the properties
% of the video display of your HMD, specifically on the video scanout order:
% Is it right to left, left to right, or top to bottom? For each scanout order
% there is an optimal order for the renderpasses to minimize perceived lag.
%
% 'targetTime' is the expected time at which the rendered frame will display.
% This could potentially be used by the driver to make better predictions of
% camera/eye/head pose for the image. Omitting the value will use a target time
% that is implementation specific, but known to give generally good results.
%
% 'userTransformMatrix' is an optional 4x4 right hand side (RHS) transformation
% matrix. It gets applied to the tracked head pose as a global transformation
% before computing results based on head pose like, e.g., camera transformations.
% You can use this to translate the "virtual head" and thereby the virtual eyes/
% cameras in the 3D scene, so observer motion is not restricted to the real world
% tracking volume of your headset. A typical 'userTransformMatrix' would be a
% combined translation and rotation matrix to position the observer at some
% 3D location in space, then define his/her global looking direction, aka as
% heading angle, yaw orientation, or rotation around the y-axis in 3D space.
% Head pose tracking results would then operate relative to this global transform.
% If 'userTransformMatrix' is left out, it will default to an identity transform,
% in other words, it will do nothing.
%
% Return values in struct 'eyePose':
%
% 'eyeIndex' The eye for which this information applies. 0 = Left eye, 1 = Right eye.
%            You can pass 'eyeIndex' into the Screen('SelectStereoDrawBuffer', win, eyeIndex)
%            to select the proper eye target render buffer.
%
% 'modelView' is a 4x4 RHS OpenGL matrix which can be directly used as OpenGL
%             GL_MODELVIEW matrix for rendering the scene.
%
% 'cameraView' contains a 4x4 RHS camera matrix which describes position and
%              orientation of the "eye camera" relative to the world reference
%              frame. It is the inverse of eyePose.modelView. This matrix can
%              be directly used to define the camera for rendering of complex
%              3D scenes with the Horde3D 3D engine or other engines which want
%              absolute camera pose instead of the inverse matrix.
%
%
% PsychVRHMD('SetupRenderingParameters', hmd [, basicTask='Tracked3DVR'][, basicRequirements][, basicQuality=0][, fov=[HMDRecommended]][, pixelsPerDisplay=1])
% - Query the HMD 'hmd' for its properties and setup internal rendering
% parameters in preparation for opening an onscreen window with PsychImaging
% to display properly on the HMD. See section about 'AutoSetupHMD' above for
% the meaning of the optional parameters 'basicTask', 'basicRequirements'
% and 'basicQuality'.
%
% 'fov' Optional field of view in degrees, from line of sight: [leftdeg, rightdeg,
% updeg, downdeg]. If 'fov' is omitted, the HMD runtime will be asked for a
% good default field of view and that will be used. The field of view may be
% dependent on the settings in the HMD user profile of the currently selected
% user. Note: Not always used with the OpenXR backend driver. See 'help PsychOpenXR'
% in the corresponding section for PsychOpenXR('SetupRenderingParameters').
%
% 'pixelsPerDisplay' Ratio of the number of render target pixels to display pixels
% at the center of distortion. Defaults to 1.0 if omitted. Lower values can
% improve performance, at lower quality.
%
%
% PsychVRHMD('SetBasicQuality', hmd, basicQuality);
% - Set basic level of quality vs. required GPU performance.
%
%
% [oldPosition, oldSize, oldOrientation] = PsychVRHMD('View2DParameters', hmd, eye [, position][, size][, orientation]);
% - Query or assign 2D quad view parameters for eye 'eye' of the hmd.
% Such 2D quad views are used in 'Monoscopic' (same view for both eyes), or
% 'Stereoscopic' mode (one view per eye), as well as in 3D modes when a script is
% 'Stop'ed and the user asked for use of these 2D quad views instead of projective
% views.
% This returns the current or previous settings for position and size in
% 'oldPosition' and 'oldSize'.
% 'eye' Mandatory: 0 = Left eye or monoscopic view, 1 = right eye in stereo mode.
% Optionally you can specify new settings, as follows:
% 'position' 3D position of the center of the virtual viewscreen, relative to the
% eye of the subject. Unit is meters, e.g., [0, 0, -0.5] would center the view at
% x,y offset zero relative to the optical axis, and 0.5 meters away from the eye.
% Iow. the center of the viewscreen aligns with the straightforward looking
% direction of the eye, but the screen floats at 0.5 meters distance. If this
% parameter is left empty [] or omitted, then the position does not change.
% Default position at session startup is centered and at a comfortable viewing
% distance away, so staring straight forward with parallel eyes, e.g., like when
% looking at an infinite point in space, would cause the center of the stimulus
% image to be located at your fixation direction.
% 'size' Size of the virtual viewscreen in meters. E.g., [0.8, 1] would have the
% screen at an apparent width of 0.8 meters and an apparent height of 1 meter. If
% the parameter is omitted or left empty [], the size won't be changed. Default
% size is 1 meter high and the width adjusted to preserve the aspect ratio of the
% Psychtoolbox onscreen window into which your script draws, so a drawn circle is
% actually circular instead of elliptic.
% 'orientation' A 4 component vector encoding a quaternion for orientation in
% space, ie. a [rx, ry, rz, rw] vector. Or for the most simple and most
% frequent use case: A rotation angle in degrees around the z-axis aka
% optical axis aka line of sight, e.g., 22.3 for a 22.3 degrees rotation.
%
%
% oldSetting = PsychVRHMD('SetFastResponse', hmd [, enable]);
% - Return old setting for 'FastResponse' mode in 'oldSetting',
% optionally disable or enable the mode via specifying the 'enable'
% parameter as 0 or > 0. Please note that if you want to use 'FastResponse',
% you must request and thereby enable it at the beginning of a session, as
% the driver must do some neccessary setup prep work at startup of the HMD.
% Once it was initially enabled, you can switch the setting at runtime with
% this function.
%
% Some drivers may implement different strategies for 'FastResponse', selectable
% via different settings for the 'enable' flag here. E.g., the Oculus Rift driver
% support three different methods 1, 2 and 3 at the moment.
%
%
% oldSetting = PsychVRHMD('SetTimeWarp', hmd [, enable]);
% - Return old setting for 'TimeWarp' mode in 'oldSetting',
% optionally enable or disable the mode via specifying the 'enable'
% parameter as 1 or 0. Please note that if you want to use 'TimeWarp',
% you must request and thereby enable it at the beginning of a session, as
% the driver must do some neccessary setup prep work at startup of the HMD.
% Once it was initially enabled, you can switch the setting at runtime with
% this function.
%
%
% oldSetting = PsychVRHMD('SetLowPersistence', hmd [, enable]);
% - Return old setting for 'LowPersistence' mode in 'oldSetting',
% optionally enable or disable the mode via specifying the 'enable'
% parameter as 1 or 0.
%
%
% PsychVRHMD('SetHSWDisplayDismiss', hmd [, dismissTypes=1+2+4]);
% - Set how the user can dismiss the "Health and safety warning display".
% 'dismissTypes' can be -1 to disable the HSWD, or a value >= 0 to show
% the HSWD until a timeout and or until the user dismisses the HSWD.
% The following flags can be added to define type of dismissal:
%
% +0 = Display until timeout, if any. Will wait forever if there isn't any timeout!
% +1 = Dismiss via keyboard keypress.
% +2 = Dismiss via mouse click or mousepad tap.
% +4 = Dismiss via a tap to the HMD (detected via accelerometer).
%
%
% [bufferSize, imagingFlags, stereoMode] = PsychVRHMD('GetClientRenderingParameters', hmd);
% - Retrieve recommended size in pixels 'bufferSize' = [width, height] of the client
% renderbuffer for each eye for rendering to the HMD. Returns parameters
% previously computed by PsychVRHMD('SetupRenderingParameters', hmd).
%
% Also returns 'imagingFlags', the required imaging mode flags for setup of
% the Screen imaging pipeline. Also returns the needed 'stereoMode' for the
% pipeline.
%
% This function is usually called by PsychImaging(), you don't need to deal
% with it.
%
%
% needPanelFitter = PsychVRHMD('GetPanelFitterParameters', hmd);
% - 'needPanelFitter' is 1 if a custom panel fitter tasks is needed, and 'bufferSize'
% from the PsychVRHMD('GetClientRenderingParameters', hmd); defines the size of the
% clientRect for the onscreen window. 'needPanelFitter' is 0 if no panel fitter is
% needed.
%
%
% [winRect, ovrfbOverrideRect, ovrSpecialFlags, ovrMultiSample] = PsychVRHMD('OpenWindowSetup', hmd, screenid, winRect, ovrfbOverrideRect, ovrSpecialFlags, ovrMultiSample);
% - Compute special override parameters for given input/output arguments, as needed
% for a specific HMD. Take other preparatory steps as needed, immediately before the
% Screen('OpenWindow') command executes. This is called as part of PsychImaging('OpenWindow'),
% with the user provided hmd, screenid, winRect etc.
%
%
% isOutput = PsychVRHMD('IsHMDOutput', hmd, scanout);
% - Returns 1 (true) if 'scanout' describes the video output to which the
% HMD 'hmd' is connected. 'scanout' is a struct returned by the Screen
% function Screen('ConfigureDisplay', 'Scanout', screenid, outputid);
% This allows probing video outputs to find the one which feeds the HMD.
%
%
% [projL, projR, fovL, fovR] = PsychVRHMD('GetStaticRenderParameters', hmd [, clipNear=0.01][, clipFar=10000.0]);
% - Retrieve parameters needed to setup the intrinsic parameters of the virtual
% camera for scene rendering.
%
% 'clipNear' Optional near clipping plane for OpenGL. Defaults to 0.01.
% 'clipFar' Optional far clipping plane for OpenGL. Defaults to 10000.0.
%
% Return arguments:
%
% 'projL' is the 4x4 OpenGL projection matrix for the left eye rendering.
% 'projR' is the 4x4 OpenGL projection matrix for the right eye rendering.
% Please note that projL and projR are usually identical for typical rendering
% scenarios.
% 'fovL' Field of view of left camera [leftAngle, rightAngle, upAngle, downAngle].
% 'fovR' Field of view of right camera [leftAngle, rightAngle, upAngle, downAngle].
% Angles are expressed in units of radians.
%
% PsychVRHMD('Start', hmd);
% - Start live operations of the 'hmd', e.g., head tracking.
%
%
% PsychVRHMD('Stop', hmd);
% - Stop live operations of the 'hmd', e.g., head tracking.
%
%

% History:
% 23-Sep-2015  mk  Written.

% Global GL handle for access to OpenGL constants needed in setup:
global GL; %#ok<NUSED,*GVMIS>

% Global OVR handle for access to VR runtime constants:
global OVR; %#ok<NUSED>

if nargin < 1 || isempty(cmd)
  help PsychVRHMD;
  return;
end

% Autodetect first connected HMD and open a connection to it. Open a
% emulated one, if none can be detected. Perform basic setup with
% default configuration, create a proper PsychImaging task.
if strcmpi(cmd, 'AutoSetupHMD')
  if length(varargin) >= 1 && ~isempty(varargin{1})
    basicTask = varargin{1};
  else
    basicTask = 'Tracked3DVR';
  end

  if length(varargin) >= 2 && ~isempty(varargin{2})
    basicRequirements = varargin{2};
  else
    basicRequirements = '';
  end

  if length(varargin) >= 3 && ~isempty(varargin{3})
    basicQuality = varargin{3};
  else
    basicQuality = 0;
  end

  if length(varargin) >= 5 && ~isempty(varargin{5})
    deviceIndex = varargin{5};
  else
    deviceIndex = [];
  end

  if length(varargin) >= 4 && ~isempty(varargin{4})
    vendor = varargin{4};
    if strcmpi(vendor, 'Oculus')
      hmd = PsychOculusVR1('AutoSetupHMD', basicTask, basicRequirements, basicQuality, deviceIndex);

      if isempty(hmd)
        hmd = PsychOculusVR('AutoSetupHMD', basicTask, basicRequirements, basicQuality, deviceIndex);
      end

      % Return the handle:
      varargout{1} = hmd;
      evalin('caller','global OVR');

      return;
    end

    if strcmpi(vendor, 'OpenHMD')
      hmd = PsychOpenHMDVR('AutoSetupHMD', basicTask, basicRequirements, basicQuality, deviceIndex);

      % Return the handle:
      varargout{1} = hmd;
      evalin('caller','global OVR');

      return;
    end

    if strcmpi(vendor, 'OpenXR')
      hmd = PsychOpenXR('AutoSetupHMD', basicTask, basicRequirements, basicQuality, deviceIndex);

      % Return the handle:
      varargout{1} = hmd;
      evalin('caller','global OVR');

      return;
    end

    error('AutoSetupHMD: Invalid ''vendor'' requested. This vendor is not supported.');
  end

  % Probe sequence:
  hmd = []; %#ok<NASGU>

  % Does user need highest timing/timestamp precision and trustworthiness?
  if ~isempty(strfind(basicRequirements, 'TimingPrecisionIsCritical')) %#ok<STREMP>
    fprintf('PsychVRHMD: INFO: Prioritizing visual stimulation timing above all else in driver selection, as requested.\n');
    needTiming = 1;
  else
    needTiming = 0;
  end

  if needTiming
    % Try to ensure best timing for users of Oculus Rift DK1 or DK2 if the
    % old v0.5 Oculus runtime is installed, active and a DK1 or DK2 is
    % connected. The original driver is the best driver wrt. quality,
    % low-level control and timing for these old HMD's, better than any
    % OpenXR, OculusVR-1 or OpenHMD driver. At least on Linux/X11 with a
    % separate X-Screen for the HMD, and for Windows as of January 2023 if
    % one can find the needed OculusVR v0.5 runtime somewhere...

    % Oculus sdk/runtime v0.5 supported and online? At least one real HMD connected?
    if PsychOculusVR('Supported') && PsychOculusVR('GetCount') > 0
      % Yes. Use that one. This will also inject a proper PsychImaging task
      % for setup of the imaging pipeline:
      try
        hmd = PsychOculusVR('AutoSetupHMD', basicTask, basicRequirements, basicQuality, deviceIndex);

        % Return the handle:
        varargout{1} = hmd;
        evalin('caller','global OVR');
        return;
      catch
        fprintf('PsychVRHMD:AutoSetupHMD: Failed to open OculusVR Rift DK1 or DK2 device, despite driver being installed and supposedly ready...\n');
      end
    end
  end

  % OpenXR supported and online? At least one real HMD connected?
  if exist('PsychOpenXR', 'file') && PsychOpenXR('Supported') && PsychOpenXR('GetCount') > 0
    % Yes. what about timing needs and capabilities of OpenXR driver?
    if ~needTiming || (PsychOpenXRCore('TimingSupport') > 0)
      % No need for best timing, or OpenXR runtime has good timing
      % facilities, so use that one. This will also inject a proper
      % PsychImaging task for setup of the imaging pipeline:
      try
        hmd = PsychOpenXR('AutoSetupHMD', basicTask, basicRequirements, basicQuality, deviceIndex);

        % Return the handle:
        varargout{1} = hmd;
        evalin('caller','global OVR');
        return;
      catch
        fprintf('PsychVRHMD:AutoSetupHMD: Failed to open OpenXR device, despite driver being installed and supposedly ready...\n');
      end
    end
  end

  % Need precise timing and no previous driver was up to it?
  if needTiming
    % OpenHMD VR supported and online? At least one real HMD connected?
    % Override: As of PTB 3.0.19.1, don't use it anymore - it is just too fickly...
    if false && PsychOpenHMDVR('Supported') && PsychOpenHMDVR('GetCount') > 0
      % Yes. Use that one. It has great timing under Linux/X11 with a
      % separate X-Screen, but restricted functionality (e.g., often no or
      % limited positional tracking) and potentially lower image quality
      % for many supported HMD's, as of OpenHMD v0.3. We don't ship the
      % driver on Windows or macOS as of PTB 3.0.19 early 2023, and don't
      % intend to do so, but at least there is the theoretical option...
      %
      % This will also inject a proper PsychImaging task for setup of the imaging pipeline:
      try
        hmd = PsychOpenHMDVR('AutoSetupHMD', basicTask, basicRequirements, basicQuality, deviceIndex);

        % Return the handle:
        varargout{1} = hmd;
        evalin('caller','global OVR');
        return;
      catch
        fprintf('PsychVRHMD:AutoSetupHMD: Failed to open OpenHMD VR HMD, despite driver being installed and supposedly ready...\n');
      end
    end

    warning('PsychVRHMD:AutoSetupHMD: Could not find any driver with the requested reliable presentation timing! Choosing OpenXR as least worst option!');

    % No dice. The last option is a bog standard OpenXR without any timing
    % enabled OpenXR runtime. At least we can use PsychOpenXRCore's builtin
    % multi-threading hacks to get not too catastrophic timing precision in
    % some cases -- hangs or crashes thoug with some buggy OpenXR
    % runtimes...
    %
    % Supported and online? At least one real HMD connected?
    if exist('PsychOpenXR', 'file') && PsychOpenXR('Supported') && PsychOpenXR('GetCount') > 0
      try
        hmd = PsychOpenXR('AutoSetupHMD', basicTask, basicRequirements, basicQuality, deviceIndex);

        % Return the handle:
        varargout{1} = hmd;
        evalin('caller','global OVR');
        return;
      catch
        fprintf('PsychVRHMD:AutoSetupHMD: Failed to open OpenXR device, despite driver being installed and supposedly ready...\n');
      end
    end

    % We reached the end of the road. No driver with even half-way
    % acceptable timing support. OculusVR-1 with its awful timing and
    % timestamping is not an option! Give up!
    warning('PsychVRHMD:AutoSetupHMD: Could not find any driver with the requested reliable presentation timing! Game over!');

    % Return an empty handle to signal lack of VR HMD support to caller,
    % so caller can cope with it somehow:
    varargout{1} = [];
    return;
  end

  % No need for great timing - continue conventional probe sequence if we
  % made it up to here...

  % Oculus runtime v1.11+ supported and online? At least one real HMD connected?
  if PsychOculusVR1('Supported') && PsychOculusVR1('GetCount') > 0
    % Yes. Use that one. This will also inject a proper PsychImaging task
    % for setup of the imaging pipeline:
    try
      hmd = PsychOculusVR1('AutoSetupHMD', basicTask, basicRequirements, basicQuality, deviceIndex);

      % Return the handle:
      varargout{1} = hmd;
      evalin('caller','global OVR');
      return;
    catch
      fprintf('PsychVRHMD:AutoSetupHMD: Failed to open OculusVR Rift DK1 or DK2 device, despite driver being installed and supposedly ready...\n');
    end
  end

  % Oculus sdk/runtime v0.5 supported and online? At least one real HMD connected?
  if PsychOculusVR('Supported') && PsychOculusVR('GetCount') > 0
    % Yes. Use that one. This will also inject a proper PsychImaging task
    % for setup of the imaging pipeline:
    try
      hmd = PsychOculusVR('AutoSetupHMD', basicTask, basicRequirements, basicQuality, deviceIndex);

      % Return the handle:
      varargout{1} = hmd;
      evalin('caller','global OVR');
      return;
    catch
      fprintf('PsychVRHMD:AutoSetupHMD: Failed to open OculusVR-1 VR HMD, despite driver being installed and supposedly ready...\n');
    end
  end

  % OpenHMD VR supported and online? At least one real HMD connected?
  if PsychOpenHMDVR('Supported') && PsychOpenHMDVR('GetCount') > 0
    % Yes. Use that one. This will also inject a proper PsychImaging task
    % for setup of the imaging pipeline:
    try
      hmd = PsychOpenHMDVR('AutoSetupHMD', basicTask, basicRequirements, basicQuality, deviceIndex);

      % Return the handle:
      varargout{1} = hmd;
      evalin('caller','global OVR');
      return;
    catch
      fprintf('PsychVRHMD:AutoSetupHMD: Failed to open OpenHMD VR HMD, despite driver being installed and supposedly ready...\n');
    end
  end

  % Add probe and autosetup calls for other HMD vendors here...

  % No success with finding any real supported HMD so far. Try to find a driver
  % that at least supports an emulated HMD for very basic testing:
  if PsychOculusVR('Supported')
    try
      hmd = PsychOculusVR('AutoSetupHMD', basicTask, basicRequirements, basicQuality, deviceIndex);
      varargout{1} = hmd;
      evalin('caller','global OVR');
      return;
    catch
      fprintf('PsychVRHMD:AutoSetupHMD: Failed to open OculusVR Rift DK1 or DK2 device, despite driver being installed and supposedly ready...\n');
    end
  end

  if PsychOpenHMDVR('Supported')
    try
      hmd = PsychOpenHMDVR('AutoSetupHMD', basicTask, basicRequirements, basicQuality, deviceIndex);
      varargout{1} = hmd;
      evalin('caller','global OVR');
      return;
    catch
      fprintf('PsychVRHMD:AutoSetupHMD: Failed to open OpenHMD VR HMD, despite driver being installed and supposedly ready...\n');
    end
  end

  % Add probe for other emulated HMD drivers here ...

  % If we reach this point then it is game over:
  fprintf('PsychVRHMD:AutoSetupHMD: Could not autosetup any HMDs, real or emulated, for any HMD vendor. Game over!\n');

  % Return an empty handle to signal lack of VR HMD support to caller,
  % so caller can cope with it somehow:
  varargout{1} = [];
  return;
end

% If the cmd could not get dispatched by us, funnel it to the
% vendor specific driver:
% 'cmd' so far not dispatched? Let's assume it is a command
% meant for the HMD specific driver:
if (length(varargin) >= 1) && isstruct(varargin{1})
  myhmd = varargin{1};
  [ varargout{1:nargout} ] = myhmd.driver(cmd, myhmd, varargin{2:end});
end

end
