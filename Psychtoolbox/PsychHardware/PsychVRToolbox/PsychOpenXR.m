function varargout = PsychOpenXR(cmd, varargin)
% PsychOpenXR - A high level driver for OpenXR supported XR hardware.
%
% Copyright (c) 2022-2023 Mario Kleiner. Licensed to you under the MIT license.
% Our underlying PsychOpenXRCore mex driver builds against the Khronos OpenXR SDK public
% headers, and links against the OpenXR open-source dynamic loader, to implement the
% interface to a system-installed OpenXR runtime. These components are dual-licensed by
% Khronos under Apache 2.0 and MIT license: SPDX license identifier "Apache-2.0 OR MIT"
%
% Note: If you want to write code that is portable across XR devices of
% different vendors, then use the PsychVRHMD() driver instead of this
% driver. The PsychVRHMD driver will use this driver as appropriate when
% connecting to a OpenXR supported XR device, but it will also
% automatically work with other head mounted displays. This driver does
% however expose a few functions specific to OpenXR hardware, so you can
% mix calls to this driver with calls to PsychVRHMD to do some mix & match.
%
% For setup instructions for OpenXR, see "help OpenXR".
%
%
% Usage:
%
% oldverbosity = PsychOpenXR('Verbosity' [, newverbosity]);
% - Get/Set level of verbosity for driver status messages, warning messages,
% error messages etc. 'newverbosity' is the optional new verbosity level,
% 'oldverbosity' is the currently set verbosity level - ie. before changing
% it.  Valid settings are: 0 = Silent, 1 = Errors only, 2 = Warnings, 3 = Info,
% 4 = Debug.
%
%
% hmd = PsychOpenXR('AutoSetupHMD' [, basicTask='Tracked3DVR'][, basicRequirements][, basicQuality=0][, deviceIndex]);
% - Open a OpenXR device, set it up with good default rendering and
% display parameters and generate a PsychImaging('AddTask', ...)
% line to setup the Psychtoolbox imaging pipeline for proper display
% on the device. This will also cause the device connection to get
% auto-closed as soon as the onscreen window which displays on
% the device is closed. Returns the 'hmd' handle of the device on success.
%
% By default, the first detected devide will be used and if no device
% is connected, it will return an empty [] hmd handle. You can override
% this default choice of device by specifying the optional 'deviceIndex'
% parameter to choose a specific device. However, only one device per machine is
% supported, so the 'deviceIndex' will probably be only useful in the future.
%
% More optional parameters: 'basicTask' what kind of task should be implemented.
% The default is 'Tracked3DVR', which means to setup for stereoscopic 3D
% rendering, driven by head motion tracking, for a fully immersive experience
% in some kind of 3D virtual world. This is the default if omitted. The task
% 'Stereoscopic' sets up for display of stereoscopic stimuli, but without
% head tracking. 'Monoscopic' sets up for display of monocular stimuli, ie.
% the device is just used as a special kind of standard display monitor. In 'Monoscopic'
% and 'Stereoscopic' mode, both eyes will be presented with an identical field of view,
% to make sure pure 2D drawing works, without the need for setup of special per-eye
% projection transformations. In 'Tracked3DVR' mode, each eye will have a different
% field of view, optimized to maximize the viewable area while still avoiding occlusion
% artifacts due to the nose of the wearer of the device.
%
% In monoscopic or stereoscopic mode, you can change the imaging parameters, ie.,
% apparent size and location of the 2D views used with the following command to
% optimize visual display:
%
% [oldPosition, oldSize, oldOrientation] = PsychOpenXR('View2DParameters', hmd, eye [, position][, size][, orientation]);
%
%
% 'basicRequirements' defines basic requirements for the task. Currently
% defined are the following strings which can be combined into a single
% 'basicRequirements' string:
%
% 'ForceSize=widthxheight' = Enforce a specific fixed size of the stimulus
% image buffer in pixels, overriding the recommmended value by the runtime,
% e.g., 'ForceSize=2200x1200' for a 2200 pixels wide and 1200 pixels high
% image buffer. By default the driver will choose values that provide good
% quality for the given XR display device, which can be scaled up or down
% with the optional 'pixelsPerDisplay' parameter for a different quality vs.
% performance tradeoff in the function PsychOpenXR('SetupRenderingParameters');
% The specified values are clamped against the maximum values supported by
% the given hardware + driver combination.
%
% 'Float16Display' = Request rendering, compositing and display in 16 bpc float
% format. This will ask Psychtoolbox to render and post-process stimuli in 16 bpc
% linear floating point format, and allocate 16 bpc half-float textures as final
% renderbuffers to be sent to the VR compositor. If the VR compositor takes advantage
% of the high source image precision is at the discretion of the compositor and device.
% By default, if this request is omitted, processing and display in sRGB format is
% requested from Psychtoolbox and the compositor, ie., a roughly gamma 2.2 8 bpc
% format is used.
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
% [oldPosition, oldSize, oldOrientation] = PsychOpenXR('View2DParameters', hmd, eye [, position][, size][, orientation]);
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
% 'NoTimingSupport' = Signal no need at all for high precision and reliability
% timing for presentation. If you don't need any timing precision or
% reliability in your script, specifying this keyword may allow the driver
% to optimize for higher performance. See 'TimingSupport' explanation right
% below:
%
% 'TimingSupport' = Use high precision and reliability timing for presentation.
%
% The current OpenXR specification, as of OpenXR version v1.0.26 from January 2023,
% does not provide any means of reliable, trustworthy, accurate timestamping of
% presentation, and all so far tested proprietary OpenXR runtime implementations
% have severely broken and defective timing support. Only the open-source
% Monado OpenXR runtime on Linux provides a reliable and accurate timing
% implementation. Therefore this driver has to use a workaround on non-Monado
% OpenXR runtimes to achieve at least ok'ish timing if you require it, and
% that workaround involves multi-threaded operation. This multi-threading
% in turn can severely degrade performance, possibly reducing achievable
% presentation framerates to (less than) half of the maximum video refresh
% rate of your device! For this reason you should only request 'TimingSupport'
% on non-Monado if you really need it and be willing to pay the performance
% price.
%
% If you omit this keyword, the driver will try to guess if you need
% precise presentation timing for your session or not. As long as you only
% call Screen('Flip', window) or Screen('Flip', window, [], ...), ie. don't
% specify a requested stimulus onset time, the driver assumes you don't
% need precise timing, just presenting as soon as possible after a
% Screen('Flip'), and also that you don't care about accurate or trustworthy
% or correct presentation timestamps to be returned by Screen('Flip'). Once
% you specify a target onset time tWhen, ie. via calling 'Flip' as
% Screen('Flip', window, tWhen [, ...]), the driver assumes from then on
% and for the rest of the session that you want reasonably accurate
% presentation timing. It will then switch to multi-threaded operation with
% better timing, but potentially drastically reduced performance.
%
% 'TimestampingSupport' = Use high precision and reliability timestamping for presentation.
%
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
%
% As mentioned here, in "help PsychVRHMD" and our "help OpenXR" overview
% and setup instructions, currently no standard OpenXR implementation with
% reliable and trustworthy timestamping exists. Proper enhancements to
% OpenXR will need to be done in the future. Right now, as of Psychtoolbox
% 3.0.19.2, we have a hacky solution for a subset of Linux users, called
% "Monado metrics timestamping hack". It goes as follows:
%
% If you need reliable timestamping, the only solution right now is to use
% Linux + the latest Monado upstream version + a modified version of Mesa + an
% AMD or Intel gpu of sufficient performance + a VR HMD supported by Monado
% on Linux. Contact our paid support "help PsychPaidSupportAndServices" for
% help in setting up this feature and getting suitable modified Monado and
% Mesa drivers. Once everything is installed on the hardware and software
% side, the following steps need to be taken at the start of each
% experiment session to enable the special Monado metrics timestamping hack
% for trustworthy timestamping at the price of lowered performance:
%
%     1. Create a Linux fifo pipe file, e.g., in a terminal type
%        "sudo mkfifo /usr/local/framequeue.protobuf"
%        You can choose any file path and name instead of
%        /usr/local/framequeue.protobuf but it makes sense to choose a
%        directory which is under your users control, not a temporary
%        directory, unless you want to repeat step 1 after each system
%        reboot. In steps 2 and 3 you must path the same path/filename to
%        both monado-service and Octave/Matlab via the XRT_METRICS_FILE
%        environment variable.
%
%     2. Start monado-service and use the created fifo file as output file
%        for the metrics log, e.g., in a terminal window via
%
%        "XRT_METRICS_FILE=/usr/local/framequeue.protobuf XRT_METRICS_EARLY_FLUSH=true monado-service"
%
%        This will launch the monado-service OpenXR compositor, enable its
%        metrics logging with low latency into the fifo, and block its
%        startup until the Psychtoolbox XR work session is started.
%
%     3. Start a PTB session, also with XRT_METRICS_FILE environment variable
%        specified to the same fifo file location during launch of Octave or
%        Matlab, e.g., in a terminal start Octave or Matlab via:
%
%        "XRT_METRICS_FILE=/usr/local/framequeue.protobuf octave --gui" or
%        "XRT_METRICS_FILE=/usr/local/framequeue.protobuf matlab"
%
%        Once the PsychOpenXR driver has detected that a Monado XR server is
%        running, and that the fifo file exists and is accessible, it opens
%        that fifo for read access, which will let monado-service fully start
%        up and get ready to serve OpenXR clients. Your Psychtoolbox session
%        should then work with trustworthy timestamps, but at potentially
%        significantly reduced performance, e.g., a framerate of only half
%        or a third of the refresh rate of your VR HMD display.
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
% 'basicQuality' defines the basic tradeoff between quality and required
% computational power. A setting of 0 gives lowest quality, but with the
% lowest performance requirements. A setting of 1 gives maximum quality at
% maximum computational load. Values between 0 and 1 change the quality to
% performance tradeoff.
%
%
% hmd = PsychOpenXR('Open' [, deviceIndex], ...);
% - Open device with index 'deviceIndex'. See PsychOpenXRCore Open?
% for help on additional parameters.
%
%
% PsychOpenXR('SetAutoClose', hmd, mode);
% - Set autoclose mode for device with handle 'hmd'. 'mode' can be
% 0 (this is the default) to not do anything special. 1 will close
% the device 'hmd' when the onscreen window is closed which displays
% on the device. 2 will do the same as 1, but close all open HMDs and
% shutdown the complete driver and OpenXR runtime - a full cleanup.
%
%
% isOpen = PsychOpenXR('IsOpen', hmd);
% - Returns 1 if 'hmd' corresponds to an open device, 0 otherwise.
%
%
% PsychOpenXR('Close' [, hmd]);
% - Close provided device 'hmd'. If no 'hmd' handle is provided,
% all HMDs will be closed and the driver will be shutdown.
%
%
% PsychOpenXR('Controllers', hmd);
% - Return a bitmask of all connected controllers: Can be the bitand
% of the OVR.ControllerType_XXX flags described in 'GetInputState'.
%
%
% info = PsychOpenXR('GetInfo', hmd);
% - Retrieve a struct 'info' with information about the device 'hmd'.
% The returned info struct contains at least the following standardized
% fields with information:
%
% handle = Driver internal handle for the specific device.
% driver = Function handle to the actual driver for the device, e.g., @PsychOpenXR.
% type   = Defines the type/vendor of the device, e.g., 'OpenXR'.
% modelName = Name string with the name of the model of the device, e.g., 'Rift DK2'.
% separateEyePosesSupported = 1 if use of PsychOpenXR('GetEyePose') will improve
%                             the quality of the VR experience, 0 if no improvement
%                             is to be expected, so 'GetEyePose' can be avoided
%                             to save processing time without a loss of quality.
%                             This *always* returns 0 on this PsychOpenXR driver.
%
% eyeTrackingSupported = Info about eye gaze tracking capabilities. A value
% of +1 means at least one gaze vector is reported. A value of +2 means
% reporting of binocular per-eye tracking data is supported. A value of
% +1024 means that HTC's proprietary SRAnipal eyetracking is available for
% more extensive gaze data reporting.
%
% The returned struct may contain more information, but the fields mentioned
% above are the only ones guaranteed to be available over the long run. Other
% fields may disappear or change their format and meaning anytime without
% warning. See 'help PsychVRHMD' for more detailed info about available fields.
%
%
% isSupported = PsychOpenXR('Supported');
% - Returns 1 if the OpenXR driver is functional, 0 otherwise. The
% driver is functional if the VR runtime library was successfully
% initialized and a connection to the VR server process has been
% established. It would return 0 if the server process would not be
% running, or if the required runtime library would not be correctly
% installed.
%
%
% [isVisible, playAreaBounds, OuterAreaBounds] = PsychOpenXR('VRAreaBoundary', hmd [, requestVisible]);
% - Request visualization of the VR play area boundary for 'hmd' and returns its
% current extents.
%
% 'requestVisible' 1 = Request showing the boundary area markers, 0 = Don't
% request showing the markers. This parameter is accepted, but ignored for OpenXR.
%
% Returns in 'isVisible' the current visibility status of the VR area boundaries.
% This driver always returns 0 for false / invisible.
%
% 'playAreaBounds' is a 3-by-n matrix defining the play area boundaries. Each
% column represents the [x;y;z] coordinates of one 3D definition point. Connecting
% successive points by line segments defines the boundary, as projected onto the
% floor. Points are listed in clock-wise direction. An empty return argument means
% that the play area is so far undefined. This driver returns empty if the boundaries
% are unknown. Otherwise it returns the bounding rectangle of the area, as current
% unextended OpenXR runtimes can only return a rectangle, not more complex boundaries.
%
% 'OuterAreaBounds' defines the outer area boundaries in the same way as
% 'playAreaBounds'. This driver currently returns the same as 'playAreaBounds', as
% current unextended OpenXR only supports that information.
%
%
% input = PsychOpenXR('GetInputState', hmd, controllerType);
% - Get input state of controller 'controllerType' associated with device 'hmd'.
%
% 'controllerType' can be one of OVR.ControllerType_LTouch, OVR.ControllerType_RTouch,
% OVR.ControllerType_Touch, OVR.ControllerType_Remote, OVR.ControllerType_XBox, or
% OVR.ControllerType_Active for selecting whatever controller is currently active.
%
% Return argument 'input' is a struct with fields describing the state of buttons and
% other input elements of the specified 'controllerType'. It has the following fields:
%
% 'Valid' = 1 if 'input' contains valid results, 0 if input status is invalid/unavailable.
% 'Time' Time of last input state change of controller.
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
% 'Thumbstick2' = Like 'Thumbstick', but for devices with a 2nd 2D input device for each hand, e.g.,
% a 2nd thumbstick or a trackpad.
%
%
% pulseEndTime = PsychOpenXR('HapticPulse', hmd, controllerType [, duration=2.5][, freq=1.0][, amplitude=1.0]);
% - Trigger a haptic feedback pulse, some controller vibration, on the
% specified 'controllerType' associated with the specified 'hmd'.
%
% Currently supported values for 'controllerType' are:
%
% OVR.ControllerType_XBox   - The Microsoft XBox controller or compatible gamepad.
% OVR.ControllerType_Remote - Connected remote control or similar, e.g., control buttons on device.
% OVR.ControllerType_LTouch - Haptic enabled left hand controller.
% OVR.ControllerType_RTouch - Haptic enabled right hand controller.
% OVR.ControllerType_Touch  - All haptics enabled hand controllers.
% OVR.ControllerType_Active - All active haptics enabled controllers.
%
% 'duration' is requested pulse duration in seconds. By default a pulse of
% 2.5 seconds duration is executed, as this is the maximum pulse duration
% supported by Oculus Rift CV1 touch controllers. Other controllers or
% OpenXR runtimes may have different limits on pulse duration, or no limit
% at all. A duration of 0 maps to the minimum duration supported by the
% active OpenXR runtime and device. 'freq' may be a normalized frequency in
% range 0.0 - 1.0, or a higher frequency in Hz. A value of 0 will disable
% an ongoing pulse. The range up to 1.0 gets mapped to the interval 0 - 320
% Hz for backwards compatibility with older Oculus VR drivers. Values
% greater than 1 are interpreted as desired frequency in Hz. OpenXR
% runtimes and hardware may clamp the requested frequency to implementation
% dependent minimum or maximum values, or quantize to only a few discrete
% frequencies. E.g., Oculus touch controllers only support 160 Hz and 320
% Hz, no other frequencies. 'amplitude' is the amplitude of the vibration
% in normalized 0.0 - 1.0 range.
%
% 'pulseEndTime' returns the expected stop time of vibration in seconds,
% given the parameters. This may be inaccurate, depending on OpenXR runtime
% and hardware.
%
% In general, unfortunately, testing so far shows that OpenXR runtimes vary
% considerably in how well they follow the requested haptic pulse duration,
% frequency, and timing, so some caution is advised wrt. haptic pulse
% feedback. Never trust a given software + hardware combo blindly, always
% verify your specific setup!
%
%
% state = PsychOpenXR('PrepareRender', hmd [, userTransformMatrix][, reqmask=1][, targetTime]);
% - Mark the start of the rendering cycle for a new 3D rendered stereoframe.
% Return a struct 'state' which contains various useful bits of information
% for 3D stereoscopic rendering of a scene, based on head tracking data.
%
% 'hmd' is the handle of the device which delivers tracking data and receives the
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
% flag means the device is actually strapped onto the subjects head and displaying
% our visual content. Lack of this flag means the device is off and thereby blanked
% and dark, or we lost access to it to another application.
%
% state also always contains a field state.SessionState, whose bits signal general
% VR session status:
% +1  = Our rendering goes to the device, ie. we have control over it. Lack of this could
%       mean the Health and Safety warning is displaying at the moment and waiting for
%       acknowledgement, or the OpenXR GUI application is in control.
% +2  = Device is present and active.
% +4  = Device is strapped onto users head. A Rift CV1 would switch off/blank if not on the head.
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
%      and orientations of left and right XR input controllers, if any.
%
%      state.handStatus(1) = Tracking status of left hand: 0 = Untracked, 1 = Orientation
%                            tracked, 2 = Position tracked, 3 = Orientation and position
%                            tracked. If handStatus is == 0 then all the following information
%                            is invalid and can not be used in any meaningful way.
%      state.handStatus(2) = Tracking status of right hand.
%
%      state.localHandPoseMatrix{1} = 4x4 OpenGL right handed reference frame matrix with
%                                     hand position and orientation encoded to define a
%                                     proper GL_MODELVIEW transform for rendering stuff
%                                     "into"/"relative to" the oriented left hand.
%      state.localHandPoseMatrix{2} = Ditto for the right hand.
%
%      state.globalHandPoseMatrix{1} = userTransformMatrix * state.localHandPoseMatrix{1};
%                                      Left hand pose transformed by passed in userTransformMatrix.
%      state.globalHandPoseMatrix{2} = Ditto for the right hand.
%
%      state.globalHandPoseInverseMatrix{1} = Inverse of globalHandPoseMatrix{1} for collision
%                                             testing/grasping of virtual objects relative to
%                                             hand pose of left hand.
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
% eyePose = PsychOpenXR('GetEyePose', hmd, renderPass [, userTransformMatrix][, targetTime]);
% - Return a struct 'eyePose' which contains various useful bits of information
% for 3D stereoscopic rendering of the stereo view of one eye, based on head or
% eye tracking data. This function provides essentially the same information as
% the 'PrepareRender' function, but only for one eye. Therefore you will need
% to call this function twice, once for each of the two renderpasses, at the
% beginning of each renderpass. NOTE: The function only exists for backwards
% compatibility with existing older VR/AR/XR scripts. It does *not* provide any
% benefit on OpenXR VR/AR/XR devices, but instead may cause a performance decrease
% when used! It is recommended to not use it in new scripts.
%
% 'hmd' is the handle of the device which delivers tracking data and receives the
% rendered content for display.
%
% 'renderPass' defines if information should be returned for the 1st renderpass
% (renderPass == 0) or for the 2nd renderpass (renderPass == 1). The driver will
% decide for you if the 1st renderpass should render the left eye and the 2nd
% pass the right eye, or if the 1st renderpass should render the right eye and
% then the 2nd renderpass the left eye. The ordering depends on the properties
% of the video display of your device, specifically on the video scanout order:
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
%            You can pass 'eyeIndex' into Screen('SelectStereoDrawBuffer', win, eyeIndex)
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
% oldType = PsychOpenXR('TrackingOriginType', hmd [, newType]);
% - Specify the type of tracking origin for OpenXR device 'hmd'.
% This returns the current type of tracking origin in 'oldType'.
% Optionally you can specify a new tracking origin type as 'newType'.
% Type must be either:
% 0 = Origin is at eye height (device height).
% 1 = Origin is at floor height.
% The eye height or floor height gets defined by the system during
% sensor calibration, possibly guided by some OpenXR GUI control application.
%
%
% PsychOpenXR('SetupRenderingParameters', hmd [, basicTask='Tracked3DVR'][, basicRequirements][, basicQuality=0][, fov=[HMDRecommended]][, pixelsPerDisplay=1])
% - Query the device 'hmd' for its properties and setup internal rendering
% parameters in preparation for opening an onscreen window with PsychImaging
% to display properly on the device. See section about 'AutoSetupHMD' above for
% the meaning of the optional parameters 'basicTask', 'basicRequirements'
% and 'basicQuality'.
%
% 'fov' Optional field of view in degrees, from line of sight: [leftdeg, rightdeg,
% updeg, downdeg]. If 'fov' is omitted, the device runtime will be asked for a
% good default field of view and that will be used. The field of view may be
% dependent on the settings in the device user profile of the currently selected
% user. Note: This parameter is ignored with the current driver in 3D mode, ie.
% basicTask '3DVR' or 'Tracked3DVR' on any standard OpenXR 1.0 backend, as the
% driver auto-selects optimal field of view for 3D perspective correct rendering.
% In the 2D modes 'Monoscopic' or 'Stereoscopic', or in 3D mode with stopped loop,
% the specified field of view will be used for calculating position and size of the
% 2D views in use. If omitted the driver will try to auto-detect a meaningful field
% of view. If that is impossible, it will use the hard-coded values of an Oculus
% Rift CV-1 HMD as fallback. In all these cases, the 'PerEyeFOV' keyword will alter
% the method of default view setup from one that only takes the minimal vertical
% field of view min(updeg, downdeg) into account and calculates horizontal size to
% preserve stimulus image aspect ratio, to one that takes all field of view parameters
% into account, even if it causes distortions of shapes.
%
% 'pixelsPerDisplay' Ratio of the number of render target pixels to display pixels
% at the center of distortion. Defaults to 1.0 if omitted. Lower values can
% improve performance, at lower quality.
%
%
% PsychOpenXR('SetBasicQuality', hmd, basicQuality);
% - Set basic level of quality vs. required GPU performance.
%
%
% oldSetting = PsychOpenXR('SetFastResponse', hmd [, enable]);
% - Return old setting for 'FastResponse' mode in 'oldSetting',
% optionally disable or enable the mode via specifying the 'enable'
% parameter as 0 or greater than zero.
%
% Deprecated: This function does nothing. It just exists for (backwards)
% compatibility with PsychVRHMD.
%
%
% oldSetting = PsychOpenXR('SetTimeWarp', hmd [, enable]);
% - Return old setting for 'TimeWarp' mode in 'oldSetting',
% optionally enable or disable the mode via specifying the 'enable'
% parameter as 1 or 0.
%
% Deprecated: This function does nothing. It just exists for (backwards)
% compatibility with PsychVRHMD.
%
%
% oldSetting = PsychOpenXR('SetLowPersistence', hmd [, enable]);
% - Return old setting for 'LowPersistence' mode in 'oldSetting',
% optionally enable or disable the mode via specifying the 'enable'
% parameter as 1 or 0.
%
% Deprecated: This function does nothing. It just exists for (backwards)
% compatibility with PsychVRHMD.
%
%
% oldSettings = PsychOpenXR('PanelOverdriveParameters', hmd [, newparams]);
% Deprecated: This function does nothing. It just exists for (backwards)
% compatibility with PsychVRHMD.
%
%
% PsychOpenXR('SetHSWDisplayDismiss', hmd [, dismissTypes=1+2+4]);
% - Set how the user can dismiss the "Health and safety warning display".
% Deprecated: This function does nothing. It just exists for (backwards)
% compatibility with PsychVRHMD.
%
%
% [bufferSize, imagingFlags, stereoMode] = PsychOpenXR('GetClientRenderingParameters', hmd);
% - Retrieve recommended size in pixels 'bufferSize' = [width, height] of the client
% renderbuffer for each eye for rendering to the device. Returns parameters
% previously computed by PsychOpenXR('SetupRenderingParameters', hmd).
%
% Also returns 'imagingFlags', the required imaging mode flags for setup of
% the Screen imaging pipeline. Also returns the needed 'stereoMode' for the
% pipeline.
%
%
% needPanelFitter = PsychOpenXR('GetPanelFitterParameters', hmd);
% - 'needPanelFitter' is 1 if a custom panel fitter task is needed, and the 'bufferSize'
% from the PsychVRHMD('GetClientRenderingParameters', hmd); defines the size of the
% clientRect for the onscreen window. 'needPanelFitter' is 0 if no panel fitter is
% needed.
%
%
% [winRect, ovrfbOverrideRect, ovrSpecialFlags, ovrMultiSample, screenid] = PsychOpenXR('OpenWindowSetup', hmd, screenid, winRect, ovrfbOverrideRect, ovrSpecialFlags, ovrMultiSample);
% - Compute special override parameters for given input/output arguments, as needed
% for a specific device. Take other preparatory steps as needed, immediately before the
% Screen('OpenWindow') command executes. This is called as part of PsychImaging('OpenWindow'),
% with the user provided hmd, screenid, winRect etc.
%
%
% isOutput = PsychOpenXR('IsHMDOutput', hmd, scanout);
% - Returns 1 (true) if 'scanout' describes the video output to which the
% device 'hmd' is connected. 'scanout' is a struct returned by the Screen
% function Screen('ConfigureDisplay', 'Scanout', screenid, outputid);
% This allows probing video outputs to find the one which feeds the device.
% Deprecated: This function does nothing. It just exists for (backwards)
% compatibility with PsychVRHMD.
%
%

% Global GL handle for access to OpenGL constants needed in setup:
global GL; %#ok<*GVMIS>
global OVR;

persistent firsttime;
persistent fmonado;
persistent oldShieldingLevel;
persistent hmd;

if nargin < 1 || isempty(cmd)
  help PsychOpenXR;
  fprintf('\n\nAlso available are functions from PsychOpenXRCore:\n');
  PsychOpenXRCore;
  return;
end

% Fast-Path function 'PresentFrame' - Present frame to VR compositor,
% wait for present completion, inject present completion timestamps:
if cmd == 1
  handle = varargin{1};
  tWhen = varargin{2};

  % Actual stimulus onset target time provided by user-script? And we know
  % that we need MT for proper timing, but this is the first time we get
  % evidence we actually need proper timing?
  if (tWhen ~= 0) && (hmd{handle}.multiThreaded == 1) && ...
     (hmd{handle}.needMTForTiming == -1 || hmd{handle}.needMTForTimestamping == -1)
    % Yes! Let's define the remainder of the session as needing full MT,
    % and enable full MT:
    if hmd{handle}.needMTForTiming == -1
      hmd{handle}.needMTForTiming = 1;
    end

    if hmd{handle}.needMTForTimestamping == -1
      hmd{handle}.needMTForTimestamping = 1;
    end

    hmd{handle}.multiThreaded = 2;
    if ~PsychOpenXRCore('PresenterThreadEnable', hmd{handle}.handle)
      PsychOpenXRCore('PresenterThreadEnable', hmd{handle}.handle, 1);
    end
    fprintf('PsychOpenXR-INFO: Need for proper timing and timestamping detected. Enabling multi-threading for remainder of session\n');
    fprintf('PsychOpenXR-INFO: to facilitate this. Performance will be reduced in exchange for better timing/timestamping.\n');
  end

  % Present and timestamp:
  [predictedOnset, hmd{handle}.predictedFutureOnset] = PsychOpenXRCore('PresentFrame', hmd{handle}.handle, tWhen);

  % Assign return values for vblTime and stimulusOnsetTime for Screen('Flip'):
  Screen('Hookfunction', hmd{handle}.win, 'SetOneshotFlipResults', '', predictedOnset, predictedOnset);

  % PresentFrame successfull and not skipped?
  if predictedOnset >= 0
    % Get fresh set of backing textures for next Screen() post-flip drawing/render
    % cycle from the OpenXR texture swap chains:
    texLeft = PsychOpenXRCore('GetNextTextureHandle', hmd{handle}.handle, 0);
    if hmd{handle}.StereoMode > 0
      texRight = PsychOpenXRCore('GetNextTextureHandle', hmd{handle}.handle, 1);
    else
      texRight = [];
    end

    if ~hmd{handle}.multiThreaded || ~hmd{handle}.needWinThreadingWa1
      % Attach them as new backing textures, detach the previously bound ones, so they
      % are ready for submission to the VR compositor:
      Screen('Hookfunction', hmd{handle}.win, 'SetDisplayBufferTextures', '', texLeft, texRight);
    end
  end

  % Workaround for SteamVR bug on Linux needed? TODO: Is this still needed?
  if hmd{handle}.steamXROpenGLWa
    % SteamVR leaves our OpenGL context in a disabled state after
    % 'EndFrameRender' aka xrReleaseSwapchainImages(), which would
    % cause OpenGL errors. The following 'GetWindowInfo' forces our
    % OpenGL context back on to resolve the problem:
    Screen('GetWindowInfo', hmd{handle}.win, -1);
  end

  return;
end

% Fast-Path function 'Cleanup' - Cleans up before onscreen window close/GL shutdown:
if cmd == 2
  handle = varargin{1};

  % Workaround for Oculus runtime on Windows, at least for NVidia gpu's, possibly others:
  while glGetError()
  end

  % Reattach old backing textures, so onscreen window can get properly destroyed:
  Screen('Hookfunction', hmd{handle}.win, 'SetDisplayBufferTextures', '', hmd{handle}.oldglLeftTex, hmd{handle}.oldglRightTex);
  hmd{handle}.oldglLeftTex = [];
  hmd{handle}.oldglRightTex = [];

  return;
end

% Check if Monado metrics support is available for our Monado timestamping hack:
if isempty(fmonado)
  fmonado = 0;

  % On Linux our own custom version of Monado can support metrics based
  % timestamping under the right conditions:
  if IsLinux
    % Does the metrics fifo file exist at the expected location? And monado-service is running?
    monadometricsfile = getenv('XRT_METRICS_FILE');
    [rc, ~] = system('pidof monado-service');
    if exist(monadometricsfile, 'file') && (rc == 0)
      % Yes. Try to open it. monado-service must have been launched with proper
      % launch options, ie.:
      % XRT_METRICS_FILE=/tmp/monado.protobuf XRT_COMPOSITOR_FORCE_GPU_INDEX=1 monado-service
      fprintf('PsychOpenXR-INFO: Waiting for monado-service connection to become ready...\n');
      fmonado = fopen(monadometricsfile, 'rb');
      if fmonado == -1
        % Failed to open file - No Monado metrics support in this session.
        fmonado = 0;
        warning('PsychOpenXR-INFO: monado-service is running and Monado metrics file exists, but can not be opened! Metrics timestamping disabled!');
      elseif isempty(getenv('MONADO_STARTED'))
        % If this is the first invocation since monado-service was launched, the
        % service may have blocked until our fopen(), so it will only now commence
        % its startup. Give it plenty seconds to get fully up and running:
        WaitSecs(10);

        % Make sure we skip this wait on a future invocation, to not make
        % the citizens restless:
        setenv('MONADO_STARTED', '1');
      end
    end

    if ~fmonado
        % No Monado metrics support - clear the marker env var, so PsychOpenXRCore
        % knows not to bother:
        setenv('XRT_METRICS_FILE', '');
    end
  end
end

if strcmpi(cmd, 'PrepareRender')
  % Get and validate handle - fast path open coded:
  myhmd = varargin{1};
  if ~((length(hmd) >= myhmd.handle) && (myhmd.handle > 0) && hmd{myhmd.handle}.open)
    error('PsychOpenXR:PrepareRender: Specified handle does not correspond to an open device!');
  end

  % Make local working copy of current hmd total state:
  myhmd = hmd{myhmd.handle};

  % Get 'userTransformMatrix' if any:
  if length(varargin) >= 2 && ~isempty(varargin{2})
    userTransformMatrix = varargin{2};
  else
    % Default: Identity transform to do nothing:
    userTransformMatrix = diag([1 1 1 1]);
  end

  % Valid: Get request mask of information to return:
  if length(varargin) >= 3 && ~isempty(varargin{3})
    reqmask = varargin{3};
  else
    % Default to: Provide basic tracking status flags, and directly useable
    % GL_MODELVIEW matrices for the cameras for rendering the left- and right-eye:
    reqmask = 1;
  end

  % Get target time for predicted camera poses, head poses etc.:
  if length(varargin) >= 4 && ~isempty(varargin{4})
    targetTime = varargin{4};
  else
    % Default: Provide predicted value for the midpoint of the next video frame:
    targetTime = [];
  end

  % Eyetracking data via SRAnipalMex requested?
  if bitand(reqmask, 4) && bitand(myhmd.eyeTrackingSupported, 1024)
    % Get latest sample from SRAnipalMex:
    srLastSample = [];
    srCalibNeeded = 0;
    [srSample, srNeedCalib, srImprove] = SRAnipalMex(5);
    srCalibNeeded = srCalibNeeded + srNeedCalib;
    while ~isempty(srSample)
      srLastSample = srSample;
      [srSample, srNeedCalib, srImprove] = SRAnipalMex(5);
      srCalibNeeded = srCalibNeeded + srNeedCalib;
    end

    while isempty(srLastSample)
      [srLastSample, srNeedCalib, srImprove] = SRAnipalMex(5);
      srCalibNeeded = srCalibNeeded + srNeedCalib;
    end

    if PsychOpenXRCore('Verbosity') > 2
      if srCalibNeeded
        fprintf('PsychOpenXR-INFO: At time %f seconds - SRAnipal eyetracker suggests a calibration might be needed.\n', GetSecs);
      end

      for i=1:length(srImprove)
        fprintf('PsychOpenXR-INFO: SRAnipal eyetracker suggests tracking improvement %i\n', srImprove(i));
      end
    end

    % Convert time in msecs to GetSecs time in seconds:
    [gaze(1).Time, gaze(2).Time, gaze(3).Time] = deal(srLastSample(2) / 1000);

    % Map eye openess to tracked status:
    if srLastSample(9) > 0 && norm(srLastSample(3:5)) > 0.1
        gaze(1).Status = 3; %#ok<*AGROW>
    else
        gaze(1).Status = 1;
    end

    if srLastSample(19) > 0 && norm(srLastSample(13:15)) > 0.1
        gaze(2).Status = 3; %#ok<*AGROW>
    else
        gaze(2).Status = 1;
    end

    if gaze(1).Status == 3 && gaze(2).Status == 3
        gaze(3).Status = 3; %#ok<*AGROW>
    else
        gaze(3).Status = 1;
    end

    if 1
      % Swap eye center / translation between left eye and right eye, to compensate
      % for a bug in the SRAnipal runtime on at least HTC Vive Pro Eye:
      gaze(1).GazePose = [srLastSample(16:18) / 1000, srLastSample(3:5)];
      gaze(2).GazePose = [srLastSample(6:8) / 1000, srLastSample(13:15)];
      % Need to switch sign of x-axis position of cyclops eye due to HTC eye switching bug above!
      srLastSample(26) = -srLastSample(26);
    else
      % Normal assignment for left and right eye:
      gaze(1).GazePose = [srLastSample(6:8) / 1000, srLastSample(3:5)]; %#ok<UNRCH> 
      gaze(2).GazePose = [srLastSample(16:18) / 1000, srLastSample(13:15)];
    end

    gaze(3).GazePose = [srLastSample(26:28) / 1000, srLastSample(23:25)];

    gaze(1).gazeEyeOpening = srLastSample(9);
    gaze(2).gazeEyeOpening = srLastSample(19);
    gaze(3).gazeEyeOpening = srLastSample(29);

    gaze(1).gazeEyePupilDiameter = srLastSample(10);
    gaze(2).gazeEyePupilDiameter = srLastSample(20);
    gaze(3).gazeEyePupilDiameter = srLastSample(30);

    % Pupil position in normalized 2D sensor space:
    gaze(1).sensor2D = srLastSample(11:12);
    gaze(2).sensor2D = srLastSample(21:22);
    gaze(3).sensor2D = srLastSample(31:32);

    % Get predicted tracking state and hand controller poses (if supported) for targetTime:
    [state, touch] = PsychOpenXRCore('GetTrackingState', myhmd.handle, targetTime, reqmask - 4);
  else
    % Get predicted eye pose, tracking state and hand controller poses (if supported) for targetTime:
    [state, touch, gaze] = PsychOpenXRCore('GetTrackingState', myhmd.handle, targetTime, reqmask);
  end

  hmd{myhmd.handle}.state = state;

  % Always return basic tracking status:
  result.tracked = state.Status;
  result.SessionState = state.SessionState;

  if bitand(state.SessionState, 8)
    % DisplayLost condition! This is an unrecoverable error. Trigger a forced session shutdown:
    error('OpenXR runtime reports loss of hardware (disconnected?) or serious malfunction. Forcing abort of this session.');
  end

  % As a bonus we return the raw eye pose vectors, given that we have them anyway:
  result.rawEyePose7{1} = state.EyePoseLeft;
  result.rawEyePose7{2} = state.EyePoseRight;

  % Want matrices which take a usercode supplied global transformation into account?
  if bitand(reqmask, 1)
    % Yes: We need tracked + predicted head pose, so we can apply the user
    % transform, and then per-eye transforms:

    % Compute per-eye global pose matrices:
    result.cameraView{1} = userTransformMatrix * eyePoseToCameraMatrix(state.EyePoseLeft);
    result.cameraView{2} = userTransformMatrix * eyePoseToCameraMatrix(state.EyePoseRight);

    % Compute inverse matrices, useable as OpenGL GL_MODELVIEW matrices for rendering:
    result.modelView{1} = inv(result.cameraView{1});
    result.modelView{2} = inv(result.cameraView{2});

    % Convert both eye poses into a head pose, both as 7-component local vector and
    % as 4x4 OpenGL right handed reference frame matrix. This is tricky or mildly wrong.
    % As head position we use the mid-point between the eye locations, ie. half-distance
    % [norm(dv(1:3)) * 0.5, 0, 0]. As orientation we use the orientation of the left eye
    % state.EyePoseLeft quaternion components 4-7. Iow. we define head pose as a copy of left eye,
    % shifted half-way along the line segment connecting the optical center of left and
    % right eye. For devices without gaze tracking, this is a reasonable approximation, as
    % they track device position and derive eye pose from device pose, so we just undo that. For
    % a device with gaze tracking that would use gaze info to compute different eye orientation
    % for each eye, this would go wrong, and something more clever would be needed, to at
    % least get a roughly correct approximation of device orientation, although an exactly
    % correct result is impossible to obtain from the two eye poses...
    dv = state.EyePoseRight - state.EyePoseLeft;
    [result.localHeadPoseMatrix, result.headPose] = eyePoseToCameraMatrix(state.EyePoseLeft, [norm(dv(1:3)) * 0.5, 0, 0]);

    % Premultiply usercode provided global transformation matrix for globalHeadPoseMatrix:
    result.globalHeadPoseMatrix = userTransformMatrix * result.localHeadPoseMatrix;
  end

  % Want matrices with tracked position and orientation of touch controllers ~ users hands?
  if bitand(reqmask, 2)
    % Yes: We need tracked + predicted hand pose, so we can apply the user
    % transform, and then per-eye transforms:

    % OpenXR 1.x SDK/runtime supports exactly 2 tracked touch controllers atm. to track users hands:
    for i=1:2
      result.handStatus(i) = touch(i).Status;

      % Bonus feature: HandPoses as 7 component translation + orientation quaternion vectors:
      result.handPose{i} = touch(i).HandPose;

      % Convert hand pose vector to 4x4 OpenGL right handed reference frame matrix:
      result.localHandPoseMatrix{i} = eyePoseToCameraMatrix(touch(i).HandPose);

      % Premultiply usercode provided global transformation matrix:
      result.globalHandPoseMatrix{i} = userTransformMatrix * result.localHandPoseMatrix{i};

      % Compute inverse matrix, maybe useable for collision testing / virtual grasping of virtual bjects:
      % Provides a transform that maps absolute geometry into geometry as "seen" from the pov of the hand.
      result.globalHandPoseInverseMatrix{i} = inv(result.globalHandPoseMatrix{i});
    end
  end

  if bitand(reqmask, 4)
    % Store raw gaze data provided by eyetracker driver in gazeRaw, can be empty:
    result.gazeRaw = gaze;

    if ~isempty(gaze)
      % Process each entry:
      for i = 1:length(gaze)
        result.gazeStatus(i) = gaze(i).Status;
        % TODO FIXME: Replacing 0 by NaN should be done in PsychOpenXRCore. Fix
        % after initial release in PTB 3.0.19.5!
        if gaze(i).Time > 0
          result.gazeTime(i) = gaze(i).Time;
        else
          result.gazeTime(i) = NaN;
        end
        result.gazeConfidence(i) = NaN;
        result.gazeEyeOpening(i) = NaN;
        result.gazeEyePupilDiameter(i) = NaN;
        result.gazeEyeConvergenceDistance = NaN;

        % Gaze tracked for this eye?
        if (gaze(i).Status == 3) && ~bitand(myhmd.eyeTrackingSupported, 1024)
          % Compute and return local gaze orientation matrix, encoding an
          % eye local reference frame, within the reference frame of the
          % XR display device (HMD):
          gazeM = eyePoseToCameraMatrix(gaze(i).GazePose);
        else
          % Not tracked or 3rd party eyetracker api. Init to a neutral identity matrix:
          gazeM = diag([1 1 1 1]);
        end

        % Valid, tracked sample from SRAnipalMex available?
        if bitand(myhmd.eyeTrackingSupported, 1024)
          if gaze(i).Status == 3
            % Override gazeM matrix with a fake matrix, based on SRAnipal data.
            % Only columns 3 and 4 for z-axis and position are valid, just enough:
            gazeM(1:3, 4) = gaze(i).GazePose(1:3);
            gazeM(1:3, 3) = gaze(i).GazePose(4:6);

            % Mysterious negation hack needed with SRAnipal:
            gazeM(1:3, 3) = -gazeM(1:3, 3);
          end

          % Store estimated eye opening and pupil diameter:
          result.gazeEyeOpening(i) = gaze(i).gazeEyeOpening;
          result.gazeEyePupilDiameter(i) = gaze(i).gazeEyePupilDiameter;

          % Distance to point of eye convergence - ie. to point of fixation:
          result.gazeEyeConvergenceDistance = srLastSample(33);
        end

        % Invert the y-Rotation subvector: Why? I don't know! But without
        % it, vertical gaze vector is wrong with the HTC Vive Pro Eye.
        % Maybe a HTC SRAnipal runtime bug?
        if myhmd.needEyeTrackingYSwitch
          gazeM(2, 1:3) = -gazeM(2, 1:3);
        end

        % Disabled, as impossible to make compatible with other implementations:
        % result.gazeLocalMatNonPortable{i} = gazeM;

        % Compute gaze ray in XR device (HMD) local reference frame:
        result.gazeRayLocal{i}.gazeC = gazeM(1:3, 4);
        result.gazeRayLocal{i}.gazeD = gazeM(1:3, 3);

        % XR display device (HMD) tracking info available?
        if bitand(reqmask, 1)
          % Compute global gaze orientation matrix and gaze ray:
          gazeM = result.globalHeadPoseMatrix * gazeM;
          % Disabled, see gazeLocalMatNonPortable result.gazeGlobalMatNonPortable{i} = gazeM;
          result.gazeRayGlobal{i}.gazeC = gazeM(1:3, 4);
          result.gazeRayGlobal{i}.gazeD = gazeM(1:3, 3);
        end

        % Try to compute 2D gaze points in onscreen window coordinates:
        handle = myhmd.handle;
        use2DViews = PsychOpenXRCore('ViewType', hmd{handle}.handle) == 0;
        [winw, winh] = Screen('WindowSize', hmd{handle}.win);

        if use2DViews
          % Map gaze vectors to 2D views, which are symetric:

          % Get local gaze ray definition:
          tv = result.gazeRayLocal{i}.gazeC;
          dv = result.gazeRayLocal{i}.gazeD;

          % Get view parameters and compute definition matrix for quadView plane:
          if hmd{handle}.StereoMode > 0 && i == 2
            % Right eye in stereo mode - Choose right eye view:
            [vpos, vsize, vorient] = PsychOpenXRCore('View2DParameters', hmd{handle}.handle, 1);
          else
            % Left eye or cyclops eye in stereo mode, or pure mono mode - Choose left eye / mono view:
            [vpos, vsize, vorient] = PsychOpenXRCore('View2DParameters', hmd{handle}.handle, 0);
          end

          MV = eyePoseToCameraMatrix([vpos, vorient]);
          % Solve ray - plane intersection between gaze ray and quadView plane:
          GM = [MV(1:3, 1), MV(1:3, 2), -dv];
          GB = tv - MV(1:3, 4);
          gaze3D = (GM \ GB)'; % Faster and more accurate implementation of gaze3D = (inv(GM) * GB)';

          % Map to normalized 2D (x,y) position in view, range [0;1] inside views area:
          gaze2D = (((gaze3D(1:2) ./ (vsize / 2)) / 2) + 0.5);
          gaze2D(1) = gaze2D(1) * winw;
          gaze2D(2) = (1 - gaze2D(2)) * winh;

          % Assign as output:
          result.gazePos{i} = gaze2D;
        elseif bitand(reqmask, 1)
          % Map gaze vectors to 3D projection layers, which are often asymetric:

          % Get local gaze ray definition:
          tv = result.gazeRayGlobal{i}.gazeC;
          dv = result.gazeRayGlobal{i}.gazeD;

          % MT defines shift of 10 meters along negative z-axis of camera
          % reference frame:
          clipNear = 10;
          MT = diag([1 1 1 1]);
          MT(3,4) = -clipNear;

          % Define plane of projectionLayer to be -10 meters away from the
          % optical center of the virtual camera, ie. translated by MT. Why
          % 10 meters? Because small values give numerical instability and
          % wrong results:

          % Left eye view, mono view or cyclops view - anything other than right eye?
          if i ~= 2
            % Left eye/projectionLayer field of view:
            fov = hmd{handle}.fovL;
            MV = result.cameraView{1} * MT;
          else
            % Right eye/projectionLayer field of view:
            fov = hmd{handle}.fovR;
            MV = result.cameraView{2} * MT;
          end

          % Solve ray - plane intersection between gaze ray and projectionLayer plane:
          GM = [MV(1:3, 1), MV(1:3, 2), -dv];
          GB = tv - MV(1:3, 4);
          gaze3D = (GM \ GB)'; % Faster and more accurate implementation of gaze3D = (inv(GM) * GB)';

          % Compute left/right/up/down distance in projectionLayer plane
          % away from (0,0) 2D center, in meters. Instead of the real
          % plane, we use a bigger plane that is 10 meters shifted away, to
          % avoid numerical problems down the road. This takes the
          % asymetric view frustum of projectionLayers into account:
          lw = tan(fov(1)) * clipNear;
          rw = tan(fov(2)) * clipNear;
          th = tan(fov(3)) * clipNear;
          bh = tan(fov(4)) * clipNear;

          % Width and height of plane in meters:
          aw = rw - lw;
          ah = th - bh;

          % Map to normalized 2D (x,y) position in view, range [0;1] inside views area:
          gaze2D = (gaze3D(1:2) - [lw, bh]) ./ [aw, ah];
          gaze2D(1) = gaze2D(1) * winw;
          gaze2D(2) = (1 - gaze2D(2)) * winh;

          % Assign as output:
          result.gazePos{i} = gaze2D;
        else
          % Assign empty output:
          result.gazePos{i} = [];
        end
      end
    else
      warning('PsychOpenXR:PrepareRender: Eye gaze tracking data requested, but gaze tracking not supported or enabled!');
    end
  end

  varargout{1} = result;

  return;
end

if strcmpi(cmd, 'GetEyePose')
  % Get and validate handle - fast path open coded:
  myhmd = varargin{1};
  if ~((length(hmd) >= myhmd.handle) && (myhmd.handle > 0) && hmd{myhmd.handle}.open)
    error('PsychOpenXR:GetEyePose: Specified handle does not correspond to an open device!');
  end

  % Valid: Get view render pass for which to return information:
  if length(varargin) < 2 || isempty(varargin{2})
    error('PsychOpenXR:GetEyePose: Required ''renderPass'' argument missing.');
  end
  renderPass = varargin{2};

  % Get 'userTransformMatrix' if any:
  if length(varargin) >= 3 && ~isempty(varargin{3})
    userTransformMatrix = varargin{3};
  else
    % Default: Identity transform to do nothing:
    userTransformMatrix = diag([1 1 1 1]);
  end

  % Get target time for predicted camera poses, head poses etc.:
  if length(varargin) >= 4 && ~isempty(varargin{4})
    targetTime = varargin{4};
  else
    % Default: Choose predicted value for onset of the next presentation frame,
    % under the assumption that we hit the flip deadline for the next video frame:
    targetTime = [];
  end

  % No preferred order of eyes wrt. renderPass - use 1:1 mapping:
  result.eyeIndex = renderPass;

  % Use general tracking function to get eye poses:
  eyes = PsychOpenXRCore('GetTrackingState', myhmd.handle, targetTime, 1);

  % Select the proper eye pose vector, depending on renderPass:
  if renderPass == 0
    result.eyePose = eyes.EyePoseLeft;
  else
    result.eyePose = eyes.EyePoseRight;
  end

  % Convert eye pose vector to 4x4 right handed camera frame matrix:
  result.localEyePoseMatrix = eyePoseToCameraMatrix(result.eyePose);

  % Premultiply usercode provided global transformation matrix for per-eye global pose matrix for this eyeIndex:
  result.cameraView = userTransformMatrix * result.localEyePoseMatrix;

  % Compute inverse matrix, useable as OpenGL GL_MODELVIEW matrix for rendering:
  result.modelView = inv(result.cameraView);

  varargout{1} = result;

  return;
end

%if strcmpi(cmd, 'GetTrackersState')
%  myhmd = varargin{1};
%  if ~((length(hmd) >= myhmd.handle) && (myhmd.handle > 0) && hmd{myhmd.handle}.open)
%    error('PsychOpenXR:GetTrackersState: Specified handle does not correspond to an open device!');
%  end
%
%  varargout{1} = PsychOpenXRCore('GetTrackersState', myhmd.handle);
%
%  return;
%end

if strcmpi(cmd, 'GetInputState')
  % Get and validate handle - fast path open coded:
  myhmd = varargin{1};
  if ~((length(hmd) >= myhmd.handle) && (myhmd.handle > 0) && hmd{myhmd.handle}.open)
    error('PsychOpenXR:GetInputState: Specified handle does not correspond to an open device!');
  end

  if length(varargin) < 2 || isempty(varargin{2})
    error('PsychOpenXR:GetInputState: Required ''controllerType'' argument missing.');
  end

  % Get input state from OpenXR:
  state = PsychOpenXRCore('GetInputState', myhmd.handle, double(varargin{2}));

  % Add some fields which are unsupported by OpenXR, but required for (backwards)
  % compatibility with PsychVRHMD. Make them simply copies of what we actually
  % get from the driver:
  state.TriggerNoDeadzone = state.Trigger;
  state.TriggerRaw = state.Trigger;
  state.GripNoDeadzone = state.Grip;
  state.GripRaw = state.Grip;
  state.ThumbstickNoDeadzone = state.Thumbstick;
  state.ThumbstickRaw = state.Thumbstick;

  varargout{1} = state;

  return;
end

if strcmpi(cmd, 'HapticPulse')
  % Get and validate handle - fast path open coded:
  myhmd = varargin{1};
  if ~((length(hmd) >= myhmd.handle) && (myhmd.handle > 0) && hmd{myhmd.handle}.open)
    error('PsychOpenXR:HapticPulse: Specified handle does not correspond to an open device!');
  end

  if length(varargin) < 2 || isempty(varargin{2})
    error('PsychOpenXR:HapticPulse: Required ''controllerType'' argument missing.');
  end

  varargout{1} = PsychOpenXRCore('HapticPulse', myhmd.handle, double(varargin{2}), varargin{3:end});

  return;
end

if strcmpi(cmd, 'Start')
  % Get and validate handle - fast path open coded:
  myhmd = varargin{1};
  if ~((length(hmd) >= myhmd.handle) && (myhmd.handle > 0) && hmd{myhmd.handle}.open)
    error('PsychOpenXR:Start: Specified handle does not correspond to an open device!');
  end

  if hmd{myhmd.handle}.switchTo2DViewsOnStop
      % Switch back to 3D projectionLayers, now that tracking is started in 3D mode:
      PsychOpenXRCore('ViewType', hmd{myhmd.handle}.handle, 1);
  end

  % Use of multi-threading only in stopped 3D mode? Then we need to stop thread now.
  if (hmd{myhmd.handle}.multiThreaded == 1) && PsychOpenXRCore('PresenterThreadEnable', hmd{myhmd.handle}.handle)
    % Stop thread:

    % Need Windows runtimes workaround?
    if hmd{myhmd.handle}.needWinThreadingWa1 && false
      texLeft = PsychOpenXRCore('GetNextTextureHandle', hmd{myhmd.handle}.handle, 0);
      if hmd{myhmd.handle}.StereoMode > 0
        texRight = PsychOpenXRCore('GetNextTextureHandle', hmd{myhmd.handle}.handle, 1);
      else
        texRight = [];
      end
    end

    % Shutdown thread, wait for it to be done:
    PsychOpenXRCore('PresenterThreadEnable', hmd{myhmd.handle}.handle, 0);

    if hmd{myhmd.handle}.needWinThreadingWa1 && false
      % Switch back to OpenXR swapchain backing textures:
      Screen('Hookfunction', hmd{myhmd.handle}.win, 'SetDisplayBufferTextures', '', texLeft, texRight);
    end
  end

  % Mark userscript driven tracking as active:
  PsychOpenXRCore('Start', hmd{myhmd.handle}.handle);

  return;
end

if strcmpi(cmd, 'Stop')
  % Get and validate handle - fast path open coded:
  myhmd = varargin{1};
  if ~((length(hmd) >= myhmd.handle) && (myhmd.handle > 0) && hmd{myhmd.handle}.open)
    error('PsychOpenXR:Stop: Specified handle does not correspond to an open device!');
  end

  % Use 2D quad views in 'Stop' mode?
  if hmd{myhmd.handle}.switchTo2DViewsOnStop
      % Switch to 2D quadView layers, now that tracking is stopped in 3D mode:
      PsychOpenXRCore('ViewType', hmd{myhmd.handle}.handle, 0);
  end

  % Use of multi-threading needed in stopped 3D mode? Either if we use
  % projection layers and they need MT updates, or if we switch to quad
  % views but they also need MT updates.
  if (hmd{myhmd.handle}.multiThreaded == 1) && hmd{myhmd.handle}.use3DMode && ...
     ((PsychOpenXRCore('NeedLocateForProjectionLayers', hmd{myhmd.handle}.handle) && ~hmd{myhmd.handle}.switchTo2DViewsOnStop) || ...
      (hmd{myhmd.handle}.switchTo2DViewsOnStop && hmd{myhmd.handle}.needMTFor2DQuadViews)) && ...
     ~PsychOpenXRCore('PresenterThreadEnable', hmd{myhmd.handle}.handle)

    % Need Windows runtimes workaround?
    if hmd{myhmd.handle}.needWinThreadingWa1 && false
      % Switch back to Screen's own backing textures:
      Screen('Hookfunction', hmd{myhmd.handle}.win, 'SetDisplayBufferTextures', '',hmd{myhmd.handle}.oldglLeftTex, hmd{myhmd.handle}.oldglRightTex);
    end

    % Start thread:
    PsychOpenXRCore('PresenterThreadEnable', hmd{myhmd.handle}.handle, 1);
  end

  % Mark userscript driven tracking as inactive:
  PsychOpenXRCore('Stop', hmd{myhmd.handle}.handle);

  return;
end

if strcmpi(cmd, 'VRAreaBoundary')
  myhmd = varargin{1};
  if ~PsychOpenXR('IsOpen', myhmd)
    error('VRAreaBoundary: Passed in handle does not refer to a valid and open device.');
  end

  % Query size of play area, build bounding rect of a valid area, otherwise return []:
  [~, spaceSize] = PsychOpenXRCore('ReferenceSpaceType', myhmd.handle);
  if ~isempty(spaceSize)
    rw = spaceSize(1);
    rh = spaceSize(2);
    playAreaBounds = [[-rw/2; 0; rh/2], [-rw/2; 0; -rh/2], [rw/2; 0; -rh/2], [rw/2; 0; rh/2]];
  else
    playAreaBounds = [];
  end

  % Return 0 for isVisible, because we simply don't know:
  [varargout{1}, varargout{2}, varargout{3}] = deal(0, playAreaBounds, playAreaBounds);
  return;
end

if strcmpi(cmd, 'TrackingOriginType')
  myhmd = varargin{1};
  if ~((length(hmd) >= myhmd.handle) && (myhmd.handle > 0) && hmd{myhmd.handle}.open)
    error('PsychOpenXR:TrackingOriginType: Specified handle does not correspond to an open device!');
  end

  varargout{1} = PsychOpenXRCore('TrackingOriginType', myhmd.handle, varargin{2:end});

  return;
end

if strcmpi(cmd, 'Supported')
  % Check if the OpenXR runtime 1+ is supported and active on this
  % installation, so it can be used to open connections to real HMDs,
  % or at least to emulate a device for simple debugging purposes:
  try
    if exist('PsychOpenXRCore', 'file') && PsychOpenXRCore('GetCount') > 0
      varargout{1} = 1;
    else
      varargout{1} = 0;
    end
  catch
    varargout{1} = 0;
  end
  return;
end

% Autodetect first connected device and open a connection to it. Open a
% emulated one, if none can be detected. Perform basic setup with
% default configuration, create a proper PsychImaging task.
if strcmpi(cmd, 'AutoSetupHMD')
  % Do we have basic runtime support?
  if ~PsychOpenXR('Supported')
    % Nope: Game over.
    fprintf('PsychOpenXR:AutoSetupHMD: Could not initialize OpenXR driver. Game over!\n');

    % Return an empty handle to signal lack of XR device support to caller,
    % so caller can cope with it somehow:
    varargout{1} = [];
    return;
  end

  % Basic task this device should fulfill:
  if length(varargin) >= 1 && ~isempty(varargin{1})
    basicTask = varargin{1};
  else
    basicTask = '';
  end

  % Basic basicRequirements to choose:
  if length(varargin) >= 2 && ~isempty(varargin{2})
    basicRequirements = varargin{2};
  else
    basicRequirements = '';
  end

  % Basic quality/performance tradeoff to choose:
  if length(varargin) >= 3 && ~isempty(varargin{3})
    basicQuality = varargin{3};
  else
    basicQuality = [];
  end

  % XR device selection:
  if length(varargin) >= 4 && ~isempty(varargin{4})
    deviceIndex = varargin{4};
    newhmd = PsychOpenXR('Open', deviceIndex);
  else
    % Check if at least one OpenXR device is connected and available:
    if PsychOpenXR('GetCount') > 0
      % Yes. Open and initialize connection to first detected device:
      fprintf('PsychOpenXR: Opening the first connected OpenXR device.\n');
      newhmd = PsychOpenXR('Open', 0);
    else
      % Device emulation not possible:
      fprintf('PsychOpenXR: No OpenXR device detected. Game over.\n');
      varargout{1} = [];
      return;
    end
  end

  % Trigger an automatic device close at onscreen window close for the device display window:
  PsychOpenXR('SetAutoClose', newhmd, 1);

  % Setup default rendering parameters:
  PsychOpenXR('SetupRenderingParameters', newhmd, basicTask, basicRequirements, basicQuality);

  % Add a PsychImaging task to use this device with the next opened onscreen window:
  PsychImaging('AddTask', 'General', 'UseVRHMD', newhmd);

  % Return the device handle:
  varargout{1} = newhmd;

  % Ready.
  return;
end

if strcmpi(cmd, 'SetAutoClose')
  myhmd = varargin{1};

  if ~PsychOpenXR('IsOpen', myhmd)
    error('PsychOpenXR:SetAutoClose: Specified handle does not correspond to an open device!');
  end

  % Assign autoclose flag:
  hmd{myhmd.handle}.autoclose = varargin{2};

  return;
end

if strcmpi(cmd, 'SetHSWDisplayDismiss')
  myhmd = varargin{1};

  if ~PsychOpenXR('IsOpen', myhmd)
    error('PsychOpenXR:SetHSWDisplay: Specified handle does not correspond to an open device!');
  end

  % Method of dismissing HSW display:
  if length(varargin) < 2 || isempty(varargin{2})
    % Default is keyboard, mouse click, or device tap:
    hmd{myhmd.handle}.hswdismiss = 1 + 2 + 4;
  else
    hmd{myhmd.handle}.hswdismiss = varargin{2};
  end

  return;
end

% Open a device:
if strcmpi(cmd, 'Open')
  if isempty(firsttime)
    firsttime = 1;
    fprintf('Copyright (c) 2022-2023 Mario Kleiner. Licensed to you under the MIT license.\n');
    fprintf('Our underlying PsychOpenXRCore mex driver builds against the Khronos OpenXR SDK public\n');
    fprintf('headers, and links against the OpenXR open-source dynamic loader, to implement the\n');
    fprintf('interface to a system-installed OpenXR runtime. These components are dual-licensed by\n');
    fprintf('Khronos under Apache 2.0 and MIT license: SPDX license identifier “Apache-2.0 OR MIT”\n\n');
  end

  [handle, modelName, runtimeName, hasEyeTracking] = PsychOpenXRCore('Open', varargin{:});

  newhmd.handle = handle;
  newhmd.driver = @PsychOpenXR;
  newhmd.type   = 'OpenXR';
  newhmd.subtype = runtimeName;
  newhmd.open = 1;
  newhmd.modelName = modelName;
  newhmd.separateEyePosesSupported = 0;
  newhmd.videoRefreshDuration = 0;
  newhmd.win = [];
  newhmd.handTrackingSupported = 1;
  newhmd.hapticFeedbackSupported = 1;
  newhmd.VRControllersSupported = 1;
  newhmd.controllerTypes = 0;
  newhmd.eyeTrackingSupported = hasEyeTracking;
  newhmd.needEyeTracking = 0;

  % Usually HMD tracking also works for mono display mode:
  newhmd.noTrackingInMono = 0;

  % Default to multiThreaded allowed:
  newhmd.multiThreaded = 1;

  % No need for MT for pure 2D mode either, aka use of quadViews, by default:
  newhmd.needMTFor2DQuadViews = 0;

  % No need for MT by default for Monado as long as Metrics logging is not used:
  newhmd.needMTForMonadoMetricsFifo = 0;

  % SteamVR OpenXR runtime needs a workaround for not properly
  % managing its OpenGL context sometimes. Needed on Linux with
  % SteamVR 1.24.6, but not on Windows:
  if IsLinux && strcmp(runtimeName, 'SteamVR/OpenXR')
    newhmd.steamXROpenGLWa = 1;
  else
    newhmd.steamXROpenGLWa = 0;
  end

  % Windows OpenXR runtimes need a special workaround in multi-threaded
  % mode to deal with severe limitations of MS-Windows OpenGL-DirectX
  % interop extension (WGL_NV_DX_interop) wrt. use of Direct3D interop
  % textures shared across multiple OpenGL contexts. Essentially, only one
  % OpenGL context at a time can lock and use such a texture, but we don't
  % have any control over locking, as this is hidden inside the OpenXR
  % runtimes (xrAcquire/Wait/ReleaseSwapchainImage implementations). With
  % the "OpenXR OpenGL work context on one thread only at a time" - which
  % is neccessarily the presenterThread in multi-threaded mode, makes it
  % almost impossible to also use such a OpenXR swapchain image texture as
  % finalizedFBO backing for Screen's stimulus rendering and post-
  % processing --> Resource locking disasters and other monsters will come
  % out! We work around this by keeping the XrSwapchain textures away from
  % Screen: Instead we use Screen's own backing textures for the
  % finalizedFBO's, only use OpenXR textures in the presenterThread, and
  % then inside the presenterThreads releaseImages function, we copy the
  % rendered stimulus from Screen's finalizedFBO texture to the OpenXR
  % swapchain texture -- one extra full stimulus image copy per present!
  % Not good for performance, but this way Screen does not have to touch
  % OpenXR stuff, and the OpenXR thread's bound OpenXR work OpenGL context
  % can read from the texture shared with Screen, and write to the OpenXR
  % textures. Cfe. special code in PsychOpenXRCore's releaseTextureHandles()
  % routine. Here we need delicate switching between the two modes of
  % operation.
  %
  % So far the theory: In practice, this only fixes mayhem on the OculusVR
  % runtime, but SteamVR still shits itself when using a secondary OpenGL
  % userspace rendering context via Screen('Begin/EndOpenGL'). May be a
  % related bug or not, but this is not it yet...
  %
  % Therefore use the workaround on OculusVR for now, but not on SteamVR:
  if IsWin && strcmp(runtimeName, 'Oculus')
    % The land of awful OpenGL-Direct3D interactions and buggy runtimes...
    newhmd.needWinThreadingWa1 = 1;
    % HMD tracking does not work for mono display mode as of OculusVR runtime 1.81.0.
    newhmd.noTrackingInMono = 1;
  else
    % Linux, where things are better, due to use of OpenGL-Vulkan interop,
    % or no need for interop at all, if XR compositors are written in
    % OpenGL. Also for SteamVR on Windows, where the workaround does not
    % help at all:
    newhmd.needWinThreadingWa1 = 0;
  end

  % Monado OpenXR runtime does not need frequent tracking to keep
  % projection layers stable and free of jitter/jerk/timeout warnings.
  if ~isempty(strfind(runtimeName, 'Monado'))
    % Monado or similar advanced: No need for this - Shaves off some
    % millisecond from a multi-threaded / not client-tracked loop and gives
    % extra visual stability:
    PsychOpenXRCore('NeedLocateForProjectionLayers', handle, 0);

    % Also no need for MT for timing (bravo Monado, you are great!):
    newhmd.needMTForTiming = 0;

    % We need MT for timestamping if we can't use tracy/metrics hacks, or a proper
    % to-be-drafted-and-prototyped Monado specifc XR timestamping extension:
    newhmd.needMTForTimestamping = -1;

    % Metrics timestamping available?
    if fmonado > 0
      % Got the file opened. That means Monado is running and properly set up for
      % Metrics logging into our fifo file and we can use the special Metrics
      % timestamping. Unfortunately when monado-service has been started with
      % Metrics output to our fifo file, we need to read data from that fifo
      % pretty much constantly, as otherwise the fifo - which has a limited capacity
      % of 1 MB - will get full and block quite quickly and things to sideways! The
      % only way to guarantee frequent enough reads is to drive this with our MT
      % thread, so we need MT at all time as soon as Monado metrics mode is active:
      newhmd.needMTForTimestamping = 0;
      newhmd.needMTForMonadoMetricsFifo = 1;
    end
  else
    % Less advanced: Need tracking update, and multi-threading if the
    % client does not use active fast tracking:
    PsychOpenXRCore('NeedLocateForProjectionLayers', handle, 1);

    % Also need multi-threading MT for timing or timestamping:
    newhmd.needMTForTiming = -1;
    newhmd.needMTForTimestamping = -1;
  end

  % SteamVR on Windows, with Oculus VR backend for Oculus HMDs?
  if IsWin && strcmpi(newhmd.modelName, 'SteamVR/OpenXR : oculus')
    % Even 2D modes with quadViews need a thread to keep them stable!
    newhmd.needMTFor2DQuadViews = -1;
  end

  % Switch of y rotation axis needed for gaze tracking math? Not by default, but
  % at least for the HTC Vive Pro Eye under Windows, with both SRAnipal native and
  % XR_EXT_eye_gaze_interaction, both based on HTC SRAnipal eye tracking. Let's
  % assume all SRAnipal eye tracked devices need this workaround:
  newhmd.needEyeTrackingYSwitch = 0;
  if IsWin && ~isempty(strfind(newhmd.modelName, 'SRanipal'))
    newhmd.needEyeTrackingYSwitch = 1;
  end

  % Default autoclose flag to "no autoclose":
  newhmd.autoclose = 0;

  % By default allow user to dismiss HSW display via key press,
  % mouse click, or device tap:
  newhmd.hswdismiss = 1 + 2 + 4;

  % Setup basic task/requirement/quality specs to "nothing":
  newhmd.basicQuality = 0;
  newhmd.basicTask = '';
  newhmd.basicRequirements = '';

  % Start with invalid future timestamps or debug timestamp:
  newhmd.predictedFutureOnset = NaN;

  if isempty(OVR)
    % Define global OVR.XXX constants:
    OVR.ControllerType_LTouch = hex2dec('0001');
    OVR.ControllerType_RTouch = hex2dec('0002');
    OVR.ControllerType_Touch = OVR.ControllerType_LTouch + OVR.ControllerType_RTouch;
    OVR.ControllerType_Remote = hex2dec('0004');
    OVR.ControllerType_XBox = hex2dec('0010');
    OVR.ControllerType_Object0 = hex2dec('0100');
    OVR.ControllerType_Object1 = hex2dec('0200');
    OVR.ControllerType_Object2 = hex2dec('0400');
    OVR.ControllerType_Object3 = hex2dec('0800');
    OVR.ControllerType_Active = hex2dec('ffffffff');

    OVR.Button_A = 1 + log2(hex2dec('00000001'));
    OVR.Button_B = 1 + log2(hex2dec('00000002'));
    OVR.Button_RThumb = 1 + log2(hex2dec('00000004'));
    OVR.Button_RShoulder = 1 + log2(hex2dec('00000008'));
    OVR.Button_X = 1 + log2(hex2dec('00000100'));
    OVR.Button_Y = 1 + log2(hex2dec('00000200'));
    OVR.Button_LThumb = 1 + log2(hex2dec('00000400'));
    OVR.Button_LShoulder = 1 + log2(hex2dec('00000800'));
    OVR.Button_Up = 1 + log2(hex2dec('00010000'));
    OVR.Button_Down = 1 + log2(hex2dec('00020000'));
    OVR.Button_Left = 1 + log2(hex2dec('00040000'));
    OVR.Button_Right = 1 + log2(hex2dec('00080000'));
    OVR.Button_Enter = 1 + log2(hex2dec('00100000'));
    OVR.Button_Back = 1 + log2(hex2dec('00200000'));
    OVR.Button_VolUp = 1 + log2(hex2dec('00400000'));
    OVR.Button_VolDown = 1 + log2(hex2dec('00800000'));
    OVR.Button_Home = 1 + log2(hex2dec('01000000'));
    OVR.Button_Private = [OVR.Button_VolUp, OVR.Button_VolDown, OVR.Button_Home];
    OVR.Button_RMask = [OVR.Button_A, OVR.Button_B, OVR.Button_RThumb, OVR.Button_RShoulder];
    OVR.Button_LMask = [OVR.Button_X, OVR.Button_Y, OVR.Button_LThumb, OVR.Button_LShoulder, OVR.Button_Enter];
    OVR.Button_MicMute = 1 + log2(hex2dec('02000000')); % PTB extension, not in original OVR spec.

    OVR.Touch_A = OVR.Button_A;
    OVR.Touch_B = OVR.Button_B;
    OVR.Touch_RThumb = OVR.Button_RThumb;
    OVR.Touch_RThumbRest = 1 + log2(hex2dec('00000008'));
    OVR.Touch_RIndexTrigger = 1 + log2(hex2dec('00000010'));
    OVR.Touch_RButtonMask = [OVR.Touch_A, OVR.Touch_B, OVR.Touch_RThumb, OVR.Touch_RThumbRest, OVR.Touch_RIndexTrigger];
    OVR.Touch_X = OVR.Button_X;
    OVR.Touch_Y = OVR.Button_Y;
    OVR.Touch_LThumb = OVR.Button_LThumb;
    OVR.Touch_LThumbRest = 1 + log2(hex2dec('00000800'));
    OVR.Touch_LIndexTrigger = 1 + log2(hex2dec('00001000'));
    OVR.Touch_LButtonMask = [OVR.Touch_X, OVR.Touch_Y, OVR.Touch_LThumb, OVR.Touch_LThumbRest, OVR.Touch_LIndexTrigger];
    OVR.Touch_RIndexPointing = 1 + log2(hex2dec('00000020'));
    OVR.Touch_RThumbUp = 1 + log2(hex2dec('00000040'));
    OVR.Touch_LIndexPointing = 1 + log2(hex2dec('00002000'));
    OVR.Touch_LThumbUp = 1 + log2(hex2dec('00004000'));
    OVR.Touch_RPoseMask =  [OVR.Touch_RIndexPointing, OVR.Touch_RThumbUp];
    OVR.Touch_LPoseMask = [OVR.Touch_LIndexPointing, OVR.Touch_LThumbUp];

    OVR.TrackedDevice_HMD        = hex2dec('0001');
    OVR.TrackedDevice_LTouch     = hex2dec('0002');
    OVR.TrackedDevice_RTouch     = hex2dec('0004');
    OVR.TrackedDevice_Touch      = OVR.TrackedDevice_LTouch + OVR.TrackedDevice_RTouch;

    OVR.TrackedDevice_Object0    = hex2dec('0010');
    OVR.TrackedDevice_Object1    = hex2dec('0020');
    OVR.TrackedDevice_Object2    = hex2dec('0040');
    OVR.TrackedDevice_Object3    = hex2dec('0080');

    OVR.TrackedDevice_All        = hex2dec('FFFF');

    OVR.KEY_USER = 'User';
    OVR.KEY_NAME = 'Name';
    OVR.KEY_GENDER = 'Gender';
    OVR.KEY_DEFAULT_GENDER = 'Unknown';
    OVR.KEY_PLAYER_HEIGHT = 'PlayerHeight';
    OVR.KEY_EYE_HEIGHT = 'EyeHeight';
    OVR.KEY_NECK_TO_EYE_DISTANCE = 'NeckEyeDistance';
    OVR.KEY_EYE_TO_NOSE_DISTANCE = 'EyeToNoseDist';

    newhmd.OVR = OVR;
    evalin('caller','global OVR');
  end

  % Store in internal array:
  hmd{handle} = newhmd;

  % Return device struct:
  varargout{1} = newhmd;
  varargout{2} = modelName;

  return;
end

if strcmpi(cmd, 'Controllers')
  myhmd = varargin{1};
  if ~PsychOpenXR('IsOpen', myhmd)
    error('Controllers: Passed in handle does not refer to a valid and open device.');
  end

  hmd{myhmd.handle}.controllerTypes = PsychOpenXRCore('Controllers', myhmd.handle);
  varargout{1} = hmd{myhmd.handle}.controllerTypes;

  return;
end

if strcmpi(cmd, 'IsOpen')
  myhmd = varargin{1};
  if (length(hmd) >= myhmd.handle) && (myhmd.handle > 0) && hmd{myhmd.handle}.open
    varargout{1} = 1;
  else
    varargout{1} = 0;
  end
  return;
end

if strcmpi(cmd, 'GetInfo')
  % Ok, cheap trick: We just return the passed in 'hmd' struct - the up to date
  % internal copy that is:
  if ~PsychOpenXR('IsOpen', varargin{1})
    error('GetInfo: Passed in handle does not refer to a valid and open device.');
  end

  myhmd = varargin{1};
  varargout{1} = hmd{myhmd.handle};

  return;
end

if strcmpi(cmd, 'Close')
  if ~isempty(varargin) && ~isempty(varargin{1})
    % Close a specific hmd device:
    myhmd = varargin{1};

    % This function can be called with the raw index handle by
    % the autoclose code path. In that case, map index back into
    % full handle struct:
    if ~isstruct(myhmd)
      if length(hmd) >= myhmd
        myhmd = hmd{myhmd};
      else
        return;
      end
    end

    if (length(hmd) >= myhmd.handle) && (myhmd.handle > 0) && hmd{myhmd.handle}.open
      PsychOpenXRCore('Close', myhmd.handle);
      hmd{myhmd.handle}.open = 0;

      % Was SRAnipalMex eyetracking active?
      if bitand(hmd{myhmd.handle}.eyeTrackingSupported, 1024) && hmd{myhmd.handle}.needEyeTracking
        % Stop tracking:
        SRAnipalMex(3);
        % Shutdown tracker connection
        SRAnipalMex(1);
      end
    end
  else
    % Shutdown whole driver:
    PsychOpenXRCore('Close');
    hmd = [];
  end

  return;
end

if strcmpi(cmd, 'IsHMDOutput')
  % This does not make much sense on OpenXR, as that runtime only supports direct
  % output mode, ie. an output completely separate from the regular desktop and
  % windowing system display space.
  varargout{1} = 0;

  return;
end

if strcmpi(cmd, 'SetBasicQuality')
  myhmd = varargin{1};
  handle = myhmd.handle;
  basicQuality = varargin{2};
  basicQuality = min(max(basicQuality, 0), 1);
  hmd{handle}.basicQuality = basicQuality;
  return;
end

if strcmpi(cmd, 'SetFastResponse')
  myhmd = varargin{1};
  if ~PsychOpenXR('IsOpen', myhmd)
    error('SetFastResponse: Passed in handle does not refer to a valid and open device.');
  end

  % FastResponse has no meaningful implementation on the OpenXR runtime, so just
  % return a constant old value of 1 for "fast response always enabled":
  varargout{1} = 1;

  return;
end

if strcmpi(cmd, 'SetTimeWarp')
  myhmd = varargin{1};
  if ~PsychOpenXR('IsOpen', myhmd)
    error('SetTimeWarp: Passed in handle does not refer to a valid and open device.');
  end

  % SetTimeWarp determined use of GPU accelerated 2D texture sampling
  % warp on the Oculus Rift DK1/DK2 with old OculusVR v0.5 SDK. On OpenXR we no
  % longer have any programmatic control over timewarping,so leave this
  % in place as dummy.

  % Return constant old setting of "TimeWarp always on":
  varargout{1} = 1;

  return;
end

if strcmpi(cmd, 'SetLowPersistence')
  myhmd = varargin{1};
  if ~PsychOpenXR('IsOpen', myhmd)
    error('SetLowPersistence: Passed in handle does not refer to a valid and open device.');
  end

  % SetLowPersistence defined the use of low persistence mode on the Rift DK2 with
  % the OculusVR v0.5 SDK and the original PsychOculusVR driver. We don't have control
  % over this on OpenXR, so for backwards compatibility, always return constant old
  % setting "Always low persistence":
  varargout{1} = 1;

  return;
end

if strcmpi(cmd, 'GetStaticRenderParameters')
  myhmd = varargin{1};

  if ~PsychOpenXR('IsOpen', myhmd)
    error('GetStaticRenderParameters: Passed in handle does not refer to a valid and open HMD.');
  end

  % Retrieve projL and projR and FoV's from driver when supported:
  if (hmd{myhmd.handle}.StereoMode > 0) || ~hmd{myhmd.handle}.noTrackingInMono
    [varargout{1}, varargout{2}] = PsychOpenXRCore('GetStaticRenderParameters', myhmd.handle, varargin{2:end});
  else
    % Fallback! Get dummy "do nothing" unity projection matrices:
    varargout{1} = diag([1 1 1 1]);
    varargout{2} = diag([1 1 1 1]);
  end

  % Get cached values of fovL and fovR:
  varargout{3} = hmd{myhmd.handle}.fovL;
  varargout{4} = hmd{myhmd.handle}.fovR;

  return;
end

if strcmpi(cmd, 'SetupRenderingParameters')
  myhmd = varargin{1};

  % Basic task this device should fulfill:
  if length(varargin) >= 2 && ~isempty(varargin{2})
    basicTask = varargin{2};
  else
    basicTask = 'Tracked3DVR';
  end

  % Basic requirements to choose:
  if length(varargin) >= 3 && ~isempty(varargin{3})
    basicRequirements = varargin{3};
  else
    basicRequirements = '';
  end

  % Basic quality/performance tradeoff to choose:
  if length(varargin) >= 4 && ~isempty(varargin{4})
    basicQuality = varargin{4};
  else
    basicQuality = 0;
  end

  if length(varargin) >= 5 && ~isempty(varargin{5})
    fov = varargin{5};
  else
    fov = [];
  end

  % Cache, so we can use (in default 2D quad view setup) or return it in
  % 'GetStaticRenderParameters':
  if ~isempty(fov)
    if ~isvector(fov) || size(fov, 2) ~= 4 || ~isnumeric(fov)
      error('SetupRenderingParameters: Invalid field of view fov specified. Not a 4 component row vector with angles in degrees.');
    end

    % Convert from absolute angles to angles with proper signs (negative
    % for leftward and downward angles) and to radians as internal unit for
    % storage and processing:
    fov = deg2rad([-fov(1), fov(2), fov(3), -fov(4)]);
  end

  hmd{myhmd.handle}.fovL = fov;
  hmd{myhmd.handle}.fovR = fov;

  if length(varargin) >= 6 && ~isempty(varargin{6})
    pixelsPerDisplay = varargin{6};
  else
    pixelsPerDisplay = 1;
  end

  hmd{myhmd.handle}.basicTask = basicTask;
  hmd{myhmd.handle}.basicRequirements = basicRequirements;

  PsychOpenXR('SetBasicQuality', myhmd, basicQuality);

  % Get optimal client renderbuffer size - the size of our virtual framebuffer for left eye:
  [hmd{myhmd.handle}.rbwidth, hmd{myhmd.handle}.rbheight, hmd{myhmd.handle}.recMSAASamples, hmd{myhmd.handle}.maxMSAASamples, hmd{myhmd.handle}.maxrbwidth, hmd{myhmd.handle}.maxrbheight] = PsychOpenXRCore('GetFovTextureSize', myhmd.handle, 0);

  % Get optimal client renderbuffer size - the size of our virtual framebuffer for right eye:
  [hmd{myhmd.handle}.rbwidth, hmd{myhmd.handle}.rbheight, hmd{myhmd.handle}.recMSAASamples, hmd{myhmd.handle}.maxMSAASamples, hmd{myhmd.handle}.maxrbwidth, hmd{myhmd.handle}.maxrbheight] = PsychOpenXRCore('GetFovTextureSize', myhmd.handle, 1);

  if pixelsPerDisplay <= 0
    sca;
    error('PsychOpenXR:SetupRenderingParameters(): Invalid ''pixelsPerDisplay'' specified! Must be greater than zero.');
  end

  % Scale runtime recommended renderbuffer width x height with
  % pixelsPerDisplay, clamp values to be at least 1 pixel and no more than
  % maximum runtime supported renderbuffer size:
  hmd{myhmd.handle}.rbwidth = min(ceil(hmd{myhmd.handle}.rbwidth * pixelsPerDisplay), hmd{myhmd.handle}.maxrbwidth);
  hmd{myhmd.handle}.rbheight = min(ceil(hmd{myhmd.handle}.rbheight * pixelsPerDisplay), hmd{myhmd.handle}.maxrbheight);

  % Forced override size of framebuffer provided?
  rbOvrSize = strfind(basicRequirements, 'ForceSize=');
  if ~isempty(rbOvrSize)
    rbOvrSize = sscanf(basicRequirements(min(rbOvrSize):end), 'ForceSize=%ix%i');
    if length(rbOvrSize) ~= 2 || ~isvector(rbOvrSize) || ~isreal(rbOvrSize)
      sca;
      error('SetupRenderingParameters(): Invalid ''ForceSize='' string in ''basicRequirements'' specified! Must be of the form ''ForceSize=widthxheight'' pixels.');
    end

    % Clamp to valid range and assign:
    hmd{myhmd.handle}.rbwidth = max(1, min(ceil(rbOvrSize(1) * pixelsPerDisplay), hmd{myhmd.handle}.maxrbwidth));
    hmd{myhmd.handle}.rbheight = max(1, min(ceil(rbOvrSize(2) * pixelsPerDisplay), hmd{myhmd.handle}.maxrbheight));
    if hmd{myhmd.handle}.rbwidth ~= rbOvrSize(1) || hmd{myhmd.handle}.rbheight ~= rbOvrSize(2)
      warning('SetupRenderingParameters(): Had to clamp ''ForceSize=widthxheight'' requested pixelbuffer size to fit into valid range! Result may look funky.');
    end
  end

  % Debug display of device output into onscreen window requested?
  if isempty(strfind(basicRequirements, 'DebugDisplay')) && isempty(oldShieldingLevel) %#ok<*STREMP>
    % No. Set to be created onscreen window to be invisible:
    oldShieldingLevel = Screen('Preference', 'WindowShieldingLevel', -1);
  end

  % Eye gaze tracking requested?
  if ~isempty(strfind(basicRequirements, 'Eyetracking'))
    if ~hmd{myhmd.handle}.eyeTrackingSupported
      warning('PsychOpenXR:SetupRenderingParameters: ''Eyetracking'' requested in ''basicRequirements'', but this XR system does not support eye tracking!');
      hmd{myhmd.handle}.needEyeTracking = 0;
    else
      hmd{myhmd.handle}.needEyeTracking = 1;
    end
  end

  return;
end

if strcmpi(cmd, 'GetClientRenderingParameters')
  myhmd = varargin{1};
  varargout{1} = [hmd{myhmd.handle}.rbwidth, hmd{myhmd.handle}.rbheight];

  % We need fast backing store support for the imaging pipeline with virtual framebuffers,
  % also output redirection for providing final output to us, instead of displaying
  % into the standard onscreen window. The OpenXR runtime generates its own swapchain
  % textures to be used as externally injected color buffer backing textures:
  imagingMode = mor(kPsychNeedFastBackingStore, kPsychNeedFinalizedFBOSinks, kPsychUseExternalSinkTextures);

  % Usercode wants a 16 bpc half-float rendering pipeline?
  if ~isempty(strfind(hmd{myhmd.handle}.basicRequirements, 'Float16Display'))
    % Request a 16 bpc float framebuffer from Psychtoolbox:
    imagingMode = mor(imagingMode, kPsychNeed16BPCFloat);
  else
    % Standard RGBA8 images: Use sRGB format for rendering/blending/compositing/display:
    imagingMode = mor(imagingMode, kPsychEnableSRGBRendering);
  end

  % Tell imaging pipeline if OpenXR compositor can receive/provide MSAA
  % textures. We claim we can if we can, and imaging pipeline wants MSAA
  % and the OpenXR compositor is capable of providing MSAA with the MSAA
  % level requested by pipeline. If the pipeline asks for a higher MSAA
  % than what the OpenXR compositor can accept/handle, then we do not claim
  % any MSAA capability. This way Screen's imaging pipeline will perform
  % MSAA internally at the wanted level and resolve down to single-sample
  % before passing the already anti-aliased content to the OpenXR
  % compositor. This way we will do the MSAA whenever we can for maximum
  % efficiency, but fallback to slow Screen MSAA if needed to fulfill the
  % user codes wishes - "quality first", at the expense of an extra framebuffer
  % copy for MSAA resolve between drawBufferFBOs and finalizedFBOs. If
  % 'DebugDisplay' mode for mirroring of device content to the onscreen window
  % is requested then we also always fallback to Screen() MSAA, instead of
  % using OpenXR MSAA, because the image mirroring code can not cope with
  % MSAA finalizedFBO's / color attachment textures, ie. glBlitFramebuffer
  % fails. Using the fallback costs performance, but makes mirroring work
  % all times:
  if (hmd{myhmd.handle}.maxMSAASamples > 1) && (hmd{myhmd.handle}.requestedScreenMSAASamples > 0) && ...
     (hmd{myhmd.handle}.requestedScreenMSAASamples <= hmd{myhmd.handle}.maxMSAASamples) && ...
     isempty(strfind(hmd{myhmd.handle}.basicRequirements, 'DebugDisplay'))
    imagingMode = mor(imagingMode, kPsychSinkIsMSAACapable);
  end

  if ~strcmpi(hmd{myhmd.handle}.basicTask, 'Monoscopic')
    % We must use stereomode 12, so we get separate draw buffers for left and
    % right eye, and separate stream processing into our XR runtime provided
    % separate backing textures / texture swapchains per eye, with all internal
    % buffers of a size that is at least full VR compositor input resolution.
    stereoMode = 12;
  else
    % Monoscopic presentation will do:
    stereoMode = 0;
  end

  varargout{2} = imagingMode;
  varargout{3} = stereoMode;
  return;
end

if strcmpi(cmd, 'GetPanelFitterParameters')
  % We don't need a custom PanelFitter task for OpenXR:
  varargout{1} = 0;
  return;
end

% [winRect, ovrfbOverrideRect, ovrSpecialFlags, ovrMultiSample, screenid] = PsychOpenXR('OpenWindowSetup', hmd, screenid, winRect, ovrfbOverrideRect, ovrSpecialFlags, ovrMultiSample);
if strcmpi(cmd, 'OpenWindowSetup')
  myhmd = varargin{1};
  screenid = varargin{2}; %#ok<NASGU>
  winRect = varargin{3};
  ovrfbOverrideRect = varargin{4}; %#ok<NASGU>
  ovrSpecialFlags = varargin{5};
  if isempty(ovrSpecialFlags)
    ovrSpecialFlags = 0;
  end

  % Get wanted MSAA level from caller:
  ovrMultiSample = varargin{6};

  % As the onscreen window is not used for displaying on the device, but
  % either not at all, or just for debug output, make it a regular GUI
  % window, managed by the window manager, so user can easily get it out
  % of the way:
  ovrSpecialFlags = mor(ovrSpecialFlags, kPsychGUIWindow + kPsychGUIWindowWMPositioned);

  % Prevent use of any functionality that requires Screen's background flipperthread,
  % e.g., Screen('AsyncFlipBegin',...) async flips, framesequential stereomode 11 and
  % of certain VRR scheduling modes for fine-grained stimulus timing. Stereomodes and
  % VRR are the domain of the OpenXR compositor in OpenXR mode, and async flips are
  % not possible because we use resources of the flipperthread for OpenGL<->OpenXR
  % interop, so the thread can't use them concurrently.
  ovrSpecialFlags = mor(ovrSpecialFlags, kPsychDontUseFlipperThread);

  % Did usercode not request a specific MSAA level?
  if isempty(ovrMultiSample)
    % No, dealers choice. Assign recommended MSAA setting from OpenXR runtime:
    if hmd{myhmd.handle}.recMSAASamples > 1
      % MSAA recommended - Assign optimal sample count:
      ovrMultiSample = hmd{myhmd.handle}.recMSAASamples;
    else
      % MSAA not recommended - Use zero value for MSAA off in Screen():
      ovrMultiSample = 0;
    end
  end

  % At this point, ovrMultiSample is either our recommended choice, or the
  % user-forced choice already passed in. We assume that our caller
  % PsychImaing will use the ovrMultiSample value, so that it will be the
  % effective minimum choice for MSAA. Store the value internally for use
  % in 'GetClientRenderingParameters' as called by PsychImaging
  % FinalizeConfiguration, to make the decision about imagingMode flags:
  hmd{myhmd.handle}.requestedScreenMSAASamples = ovrMultiSample;

  % The current design iteration requires the PTB parent onscreen windows
  % effective backbuffer (from the pov of the imaging pipeline) to have the
  % same size (width x height) as the renderbuffer for one eye, so enforce
  % that constraint by setting ovrfbOverrideRect accordingly.

  % Get required output buffer size:
  clientRes = myhmd.driver('GetClientRenderingParameters', myhmd);

  % Set as fbOverrideRect for window:
  ovrfbOverrideRect = [0, 0, clientRes(1), clientRes(2)];

  fprintf('PsychOpenXR-INFO: Overriding onscreen window framebuffer size to %i x %i pixels for use with XR device direct output mode.\n', ...
          clientRes(1), clientRes(2));

  % Skip all visual timing sync tests and calibrations, as display timing
  % of the onscreen window doesn't matter, only the timing on the device direct
  % output matters - and that can't be measured by our standard procedures:
  Screen('Preference', 'SkipSyncTests', 2);

  varargout{1} = winRect;
  varargout{2} = ovrfbOverrideRect;
  varargout{3} = ovrSpecialFlags;
  varargout{4} = ovrMultiSample;
  varargout{5} = screenid;

  return;
end

if strcmpi(cmd, 'PerformPostWindowOpenSetup')
  % Must have global GL constants:
  if isempty(GL)
    varargout{1} = 0;
    warning('PTB internal error in PsychOpenXR: GL struct not initialized?!?');
    return;
  end

  % OpenXR device handle:
  myhmd = varargin{1};
  handle = myhmd.handle;

  % Onscreen window handle:
  win = varargin{2};
  winfo = Screen('GetWindowInfo', win);
  hmd{handle}.StereoMode = winfo.StereoMode;

  % Keep track of window handle of associated onscreen window:
  hmd{handle}.win = win;

  % Restore shielding level for new windows after "our" onscreen window is now open:
  if ~isempty(oldShieldingLevel)
    Screen('Preference', 'WindowShieldingLevel', oldShieldingLevel);
    oldShieldingLevel = [];
  end

  % Need to know user selected clearcolor:
  clearcolor = varargin{3};

  % Create texture swap chains to provide textures to be used for
  % frame submission to the VR compositor:
  if ~isempty(strfind(hmd{handle}.basicRequirements, 'Float16Display'))
    % Linear RGBA16F half-float textures as target framebuffers:
    floatFlag = 1;
  else
    % sRGB RGBA8 textures as target framebuffers:
    floatFlag = 0;
  end

  if ~isempty(strfind(hmd{handle}.basicTask, '3D'))
    % 3D rendering task:
    hmd{handle}.use3DMode = 1;
  else
    % No 3D rendering, just monoscopic or stereoscopic display of stimuli:
    hmd{handle}.use3DMode = 0;
  end

  % SteamVR runtime on MS-Windows in 3D mode needs fp16 buffers for some
  % absurd reason, most likely some bug in handling of projection layers.
  % This workaround is not needed in 2D mode where quad layers are used.
  % This at least as of December 2022 with SteamVR version 1.24.7, when using
  % OculusVR as backend to drive Oculus devices. Not clear if also needed for
  % other vendors HMDs. Go figure!
  if IsWin && hmd{handle}.use3DMode && strcmpi(hmd{handle}.modelName, 'SteamVR/OpenXR : oculus')
    fprintf('PsychOpenXR-INFO: Using floating point textures for SteamVR runtime with Oculus devices in 3D mode on MS-Windows.\n');
    fprintf('PsychOpenXR-INFO: This is a workaround for some SteamVR bug. It may cause reduced performance, sorry.\n');
    floatFlag = 1;
  end

  % Now for the decision making if we always/never/conditionally need
  % multi-threading (MT) during this session. MT can have a substantial
  % impact on performance, and with some buggy OpenXR runtimes on
  % stability, so we want to avoid it whenever possible.

  % Does user want us to not care about visual glitch prevention in 'Stop' mode or slow running
  % scripts? If so, we can avoid all use of multi-threading for such purposes.
  if ~isempty(strfind(hmd{handle}.basicRequirements, 'DontCareAboutVisualGlitchesWhenStopped'))
    % Don't use MT for 3D projection layers when 'Stop'ed:
    if PsychOpenXRCore('NeedLocateForProjectionLayers', handle) || (hmd{handle}.needMTFor2DQuadViews ~= 0)
      fprintf('PsychOpenXR-INFO: Avoiding use of multi-threading for stopped or slow 2D/3D animation loops on this runtime,\n');
      fprintf('PsychOpenXR-INFO: as requested via DontCareAboutVisualGlitchesWhenStopped keyword. Visual glitches may occur.\n');
    end

    PsychOpenXRCore('NeedLocateForProjectionLayers', handle, 0);
    hmd{handle}.needMTFor3DViews = 0;
    hmd{handle}.needMTFor2DQuadViews = 0;
  end

  % User wants us to switch to 2D quad views in 'Stop' mode?
  if ~isempty(strfind(hmd{handle}.basicRequirements, 'Use2DViewsWhen3DStopped')) && hmd{handle}.use3DMode
    hmd{handle}.switchTo2DViewsOnStop = 1;
  else
    hmd{handle}.switchTo2DViewsOnStop = 0;
  end

  % 3D mode with projectionLayers, and need continuous tracking updates when stopped?
  if PsychOpenXRCore('NeedLocateForProjectionLayers', handle) && hmd{handle}.use3DMode && ...
     isempty(strfind(hmd{handle}.basicRequirements, 'Use2DViewsWhen3DStopped'))
    hmd{handle}.needMTFor3DViews = -1;
    fprintf('PsychOpenXR-INFO: Will need multi-threading for stopped 3D animation loops on this runtime. Performance will be mildly reduced throughout the session.\n');
    if hmd{handle}.needMTFor2DQuadViews == 0
      fprintf('PsychOpenXR-INFO: Consider using the keyword Use2DViewsWhen3DStopped in the basicRequirements parameter for possibly optimized performance.\n');
    end
  else
    hmd{handle}.needMTFor3DViews = 0;
  end

  % Will 2D quadViews be needed in this session, and do they need
  % multi-threading to stay stable?
  if hmd{handle}.needMTFor2DQuadViews ~= 0
    % 2D mode active, which needs permanent MT in this case?
    if ~hmd{handle}.use3DMode
      % Yes. Make it so:
      hmd{handle}.needMTFor2DQuadViews = 1;
      fprintf('PsychOpenXR-INFO: Will need multi-threading even for 2D Monoscopic/Stereoscopic mode on this runtime. Performance will be reduced throughout the session.\n');
    elseif ~isempty(strfind(hmd{handle}.basicRequirements, 'Use2DViewsWhen3DStopped'))
      % 3D mode: Only need MT for quadViews if PsychVRHMD('Stop'):
      hmd{handle}.needMTFor2DQuadViews = -1;
      fprintf('PsychOpenXR-INFO: Will need multi-threading for stopped 3D animation loops on this runtime. Performance will be mildly reduced throughout the session.\n');
    else
      % No need at all, as we are in 3D mode and user doesn't want to use quadViews when stopped:
      hmd{handle}.needMTFor2DQuadViews = 0;
    end
  end

  % Specific NoTimingSupport or TimingSupport requested for this session?
  if ~isempty(strfind(hmd{handle}.basicRequirements, 'NoTimingSupport'))
    % Absolutely no timing support needed -> Disable permanently:
    hmd{handle}.needMTForTiming = 0;
  elseif ~isempty(strfind(hmd{handle}.basicRequirements, 'TimingSupport'))
    % Timing support definitely needed -> Enable permanently on all
    % runtimes that need MT for it:
    if hmd{handle}.needMTForTiming ~= 0
      hmd{handle}.needMTForTiming = 1;
      fprintf('PsychOpenXR-INFO: Will need multi-threading for proper frame presentation timing on this runtime. Performance will be reduced throughout the session.\n');
    end
  elseif hmd{handle}.needMTForTiming ~= 0
    fprintf('PsychOpenXR-INFO: Need for multi-threading for proper frame presentation timing during this session unknown. Multi-threading will be enabled\n');
    fprintf('PsychOpenXR-INFO: if i can determine a need for it. Performance will be mildly reduced throughout the session. Please use the keywords\n');
    fprintf('PsychOpenXR-INFO: TimingSupport or NoTimingSupport in the basicRequirements parameter to give me a clue about what you want, for\n');
    fprintf('PsychOpenXR-INFO: both potentially higher reliability of timing and timestamping, and potentially better performance.\n');
  end

  % Specific NoTimestampingSupport or TimestampingSupport requested for this session?
  if ~isempty(strfind(hmd{handle}.basicRequirements, 'NoTimestampingSupport'))
    % Absolutely no timestamping support needed -> Disable permanently:
    hmd{handle}.needMTForTimestamping = 0;
  elseif ~isempty(strfind(hmd{handle}.basicRequirements, 'TimestampingSupport'))
    % Timestamping support definitely needed -> Enable permanently on all
    % runtimes that need MT for it:
    if hmd{handle}.needMTForTimestamping ~= 0
      hmd{handle}.needMTForTimestamping = 1;
      fprintf('PsychOpenXR-INFO: Will need multi-threading for proper frame timestamping on this runtime. Performance will be reduced throughout the session.\n');
    end
  elseif hmd{handle}.needMTForTimestamping ~= 0
    fprintf('PsychOpenXR-INFO: Need for multi-threading for better frame timestamping during this session unknown. Timestamps will be unreliable until i\n');
    fprintf('PsychOpenXR-INFO: can determine need for proper timing. Performance will be mildly reduced throughout the session. Please use the keywords\n');
    fprintf('PsychOpenXR-INFO: TimestampingSupport or NoTimestampingSupport in the basicRequirements parameter to give me a clue about what you want, for\n');
    fprintf('PsychOpenXR-INFO: both potentially higher timestamp trustworthiness and potentially better performance.\n');
  end

  if hmd{handle}.needMTForMonadoMetricsFifo
    fprintf('PsychOpenXR-INFO: Monado supports metrics timestamping in this session. Will need permanent multi-threading.\n');
  end

  % Derive initial master multiThreaded mode from current MT requirements:
  mtReqs = [hmd{handle}.needMTFor3DViews, hmd{handle}.needMTFor2DQuadViews, hmd{handle}.needMTForTiming, hmd{handle}.needMTForTimestamping, hmd{handle}.needMTForMonadoMetricsFifo];
  if ~any(mtReqs) || ~isempty(strfind(hmd{handle}.basicRequirements, 'ForbidMultiThreading'))
    % No need or want for multi-threading at all in this session -> Master
    % disable. We run only single-threaded for this session, which is
    % most trouble-free and efficient/high-perf:
    hmd{handle}.multiThreaded = 0;

    % Will this have downsides?
    if any(mtReqs)
      % Yep:
      fprintf('PsychOpenXR-WARNING: User script forbids any use of multi-threading for its use-case, but would be needed! Expect timing/timestamping/jitter/judder problems!\n');
    end
  else
    % Some potential need for MT. Ok to use?
    if hmd{handle}.multiThreaded == 0
      fprintf('PsychOpenXR-WARNING: User script needs multi-threading for its use-case, but multi-threading is disabled! Expect timing/timestamping/jitter/judder problems!\n');
    else
      % Special troublemakers? SteamVR on Windows, as of version 2.0.10
      % from November 2023 will cause Matlab to hang / fail / malfunction if
      % Screen('BeginOpenGL') is used for typical 3D rendering, unless
      % OpenGL context isolation is disabled, which is a troublemaker in
      % many other ways! Bug confirmed for both OculusVR backend with
      % Oculus Rift CV-1, and Vive backend with HTC Vive Pro Eye. Tested on
      % both AMD and NVidia graphics:
      if IsWin && hmd{handle}.use3DMode && strcmpi(hmd{handle}.subtype, 'SteamVR/OpenXR')
        fprintf('PsychOpenXR-WARNING: User script needs multi-threading for its use-case, but broken MS-Windows SteamVR OpenXR runtime in use!\n');
        % kPsychDisableContextIsolation in use?
        if bitand(Screen('Preference', 'ConserveVRAM'), 8)
          fprintf('PsychOpenXR-WARNING: I see you disabled OpenGL context isolation to work around the problem. Tread carefully, this\n');
          fprintf('PsychOpenXR-WARNING: may screw up rendering and Screen() operation badly if you don''t know exactly what you are doing!\n');
        else
          % This is an almost guaranteed crasher as of SteamVR 2.0.10 from
          % November 2023 - it will fail after a few seconds of 3D rendering
          % with a hard hang of Matlab and one needs to kill the
          % application via task manager etc.:
          fprintf('PsychOpenXR-WARNING: As of SteamVR version 2.0.10 from November 2023, this will almost certainly end in a Psychtoolbox hang or crash\n');
          fprintf('PsychOpenXR-WARNING: if your script calls Screen(''BeginOpenGL'') anywhere. Brace for impact! Report back if you do not experience any\n');
          fprintf('PsychOpenXR-WARNING: problems with a later/future SteamVR version.\n');
        end
      end
    end

    % Is there a definite request to use it throughout this session?
    if any(mtReqs > 0)
      % Yes, at least one mandatory user of permanent MT. Keep it on all time:
      hmd{handle}.multiThreaded = 2;
    else
      % No, only some conditional on-demand use. Keep it on dynamic/on-demand:
      hmd{handle}.multiThreaded = 1;
    end
  end

  % Create and startup XR session, based on the Screen() OpenGL interop info in 'gli':
  gli = Screen('GetWindowInfo', win, 9);

  % Multithreaded operation, with a separate OpenXR frame worker thread
  % inside PsychOpenXRCore? This would need a dedicated interop OpenGL
  % context assigned to that thread only:
  if hmd{handle}.multiThreaded
    % Use dedicated OpenGL context for OpenXR worker thread:
    openglContext = gli.OpenGLContext;

    if hmd{handle}.multiThreaded == 2
      fprintf('PsychOpenXR-INFO: Multithreaded mode permanently active due to timing/timestamping needs, or for some workaround, see above. Performance will be likely reduced.\n');
    else
      fprintf('PsychOpenXR-INFO: Multithreaded mode on-demand selected due to timing/timestamping needs, or for some workaround, see above. Performance can be reduced sometimes.\n');
    end
  else
    % Use Screen()'s main OpenGL context for everything, both Screen and OpenXR OpenGL ops:
    openglContext = gli.OpenGLContextScreen;

    fprintf('PsychOpenXR-INFO: Purely single-threaded mode active. Great for performance!\n');
  end

  % Query currently bound finalizedFBO backing textures, to keep them around as backups for restoration when closing down the session:
  [hmd{handle}.oldglLeftTex, hmd{handle}.oldglRightTex, textarget, texformat, texmultisample, texwidth, texheight, fboIds(1), fboIds(2)] = Screen('Hookfunction', win, 'GetDisplayBufferTextures');

  % The MS-Windows workaround needs this extra copy operation:
  if hmd{handle}.multiThreaded && hmd{handle}.needWinThreadingWa1
    bufferHandles = [hmd{handle}.oldglLeftTex, hmd{handle}.oldglRightTex, fboIds(1), fboIds(2)];
  else
    bufferHandles = [];
  end

  % Create and start OpenXR session:
  [hmd{handle}.videoRefreshDuration] = PsychOpenXRCore('CreateAndStartSession', hmd{handle}.handle, gli.DeviceContext, openglContext, gli.OpenGLDrawable, ...
                                                                                gli.OpenGLConfig, gli.OpenGLVisualId, hmd{handle}.use3DMode, hmd{handle}.multiThreaded, bufferHandles);

  % Set override window parameters with pixel size (color depth) and refresh interval as provided by the XR runtime:
  Screen('HookFunction', win, 'SetWindowBackendOverrides', [], 24, hmd{handle}.videoRefreshDuration);

  % Validate texture internal formats. Ideally something supportable by XR runtime, but
  % if not, it will simply trigger fbo unsharing and a slight performance loss:
  if ~ismember(texformat, [GL.RGBA8, GL.RGBA16F, GL.RGBA16, GL.RGBA16_SNORM, GL.RGBA32F])
    sca;
    error('Invalid Screen() backing textures required. Non-matching texture internal format.');
  end

  % Validate MSAA anti-aliasing support. If XR can not do MSAA then Screen
  % must not require that:
  if (hmd{handle}.maxMSAASamples <= 1 && (textarget ~= GL.TEXTURE_2D || texmultisample ~= 0))
    sca;
    error('Invalid Screen() backing textures required. Screen() assumes MSAA XR compositor target textures, but XR textures are only non-MSAA capable.');
  end

  % If XR can do MSAA, then Screen can use require MSAA or non-MSAA,
  % depending on user codes needs or imaging pipeline configuration. E.g.,
  % if complex panel-fitting or image post-processing is needed, then even
  % a MSAA configured Screen may do internal MSAA resolve and require
  % non-MSAA GL_TEXTURE_2D. For MSAA with no real image processing, Screen
  % will likely require XR to provide MSAA GL_TEXTURE_2D_MULTISAMPLE
  % instead for higher efficiency and zero-copy operation:
  if hmd{handle}.maxMSAASamples > 1 && ~ismember(textarget, [GL.TEXTURE_2D_MULTISAMPLE, GL.TEXTURE_2D])
    sca;
    error('Invalid Screen() backing textures required. Not GL_TEXTURE_2D or GL_TEXTURE_2D_MULTISAMPLE, which is incompatible.');
  end

  % External MSAA implies at least 2 samples per target texture texel:
  if textarget == GL.TEXTURE_2D_MULTISAMPLE && texmultisample < 2
    texmultisample = 2;
    fprintf('PsychOpenXR-WARNING: Invalid Screen() backing textures required: GL_TEXTURE_2D_MULTISAMPLE, but with less than 2 samples! Force-upgrading to 2 samples!\n');
  end

  % We have a valid MSAA or non-MSAA config. Lets see what Screen wants:
  if textarget == GL.TEXTURE_2D_MULTISAMPLE
    % Screen wants MSAA GL_TEXTURE_2D_MULTISAMPLE textures for texmultisample-MSAA
    % with zero-copy redirection for minimal overhead and best quality. See if
    % we can give it as many texmultisample samples as it ideally wants,
    % otherwise clamp to XR compositor supported maximum and warn about
    % slightly degraded quality:
    if texmultisample > hmd{handle}.maxMSAASamples
      fprintf('PsychOpenXR-INFO: Screen would like %i-MSAA backing textures, but OpenXR compositor can only do %i-MSAA.\n', texmultisample, hmd{handle}.maxMSAASamples);
      fprintf('PsychOpenXR-INFO: Clamping to compositor maximum of %i-MSAA. Quality could be slightly degraded.\n', hmd{handle}.maxMSAASamples);
      hmd{handle}.texmultisample = hmd{handle}.maxMSAASamples;
    else
      hmd{handle}.texmultisample = texmultisample;
    end
  else
    % Screen wants non-MSAA GL_TEXTURE_2D textures and either does not use
    % MSAA at all, or resolves down from MSAA to non-MSAA internally. We
    % need to provide single-sampled textures:
    hmd{handle}.texmultisample = 1;
  end

  if hmd{handle}.texmultisample > 1
    fprintf('PsychOpenXR-INFO: Using %i-MSAA anti-aliasing for XR compositor.\n', hmd{handle}.texmultisample);
  end

  if hmd{handle}.texmultisample ~= hmd{handle}.recMSAASamples
    fprintf('PsychOpenXR-INFO: Chosen %i-MSAA anti-aliasing for XR compositor does not match runtime recommended value %i-MSAA.\n', hmd{handle}.texmultisample, hmd{handle}.recMSAASamples);
    fprintf('PsychOpenXR-INFO: You may want to adjust that for an optimal performance vs. quality tradeoff.\n');
  end

  if hmd{handle}.oldglRightTex == 0
    hmd{handle}.oldglRightTex = [];
  end

  % Create left eye / mono OpenXr swapchain:
  [width, height, ~, texChainFormat] = PsychOpenXRCore('CreateRenderTextureChain', hmd{handle}.handle, 0, hmd{handle}.rbwidth, hmd{handle}.rbheight, floatFlag, hmd{handle}.texmultisample);

  % Create 2nd chain for right eye in stereo mode:
  if winfo.StereoMode > 0
    if winfo.StereoMode ~=12
      sca;
      error('Invalid Screen() StereoMode in use for OpenXR! Must be mode 12.');
    end
    [width, height, ~, texChainFormat] = PsychOpenXRCore('CreateRenderTextureChain', hmd{handle}.handle, 1, hmd{handle}.rbwidth, hmd{handle}.rbheight, floatFlag, hmd{handle}.texmultisample);
  end

  if (texwidth ~= width) || (texheight ~= height)
    sca;
    fprintf('PsychOpenXR-ERROR: Backing texture size mismatch: %i x %i != %i x %i!\n', texwidth, texheight, width, height);
    error('Invalid Screen() backing textures required. Non-matching width x height.');
  end

  % Check if requested swapchain format is supported by runtime:
  if floatFlag && ~ismember(texChainFormat, [GL.RGBA16F, GL.RGBA32F, GL.RGB16F])
      % No. Screen() will have to unshare the drawBufferFBO and
      % finalizedFBO to work around this and retain as much of the
      % precision and range of 16 bit float as possible. Output precision
      % will be reduced, and overhead for an extra blit will have to be
      % paid. Warn about this:
      fprintf('PsychOpenXR-WARNING: OpenXR runtime does not support the requested Float16Display basic requirement. Working around it, but precision and performance will suffer.\n');
  end

  % Make sure our OpenGL rendering context is bound for the window properly:
  Screen('GetWindowInfo', win);

  % Get first textures for actual use in PTB's imaging pipeline:
  texLeft = PsychOpenXRCore('GetNextTextureHandle', hmd{handle}.handle, 0);
  if hmd{handle}.StereoMode > 0
      texRight = PsychOpenXRCore('GetNextTextureHandle', hmd{handle}.handle, 1);
  else
      texRight = [];
  end

  % Assign them to Screen(), tell Screen the true texture format, so it can adapt if needed:
  if ~hmd{handle}.multiThreaded || ~hmd{handle}.needWinThreadingWa1
    Screen('Hookfunction', win, 'SetDisplayBufferTextures', '', texLeft, texRight, [], texChainFormat);
  end

  % Go back to user requested clear color, now that all our buffers
  % are cleared to black:
  Screen('FillRect', win, clearcolor);

  % Define parameters for the ongoing Psychtoolbox onscreen window flip operation:
  % Debug display of device output into onscreen window requested?
  if ~isempty(strfind(hmd{handle}.basicRequirements, 'DebugDisplay'))
    % Debug output of compositor mirror texture into PTB onscreen window requested.
    % - Ask to skip flip's regular OpenGL swap completion timestamping, but instead
    %   to accept future injected timestamps from us.
    %
    % - Ask to disable vsync of the OpenGL bufferswap for display of the mirror texture
    %   in the onscreen window. We don't want to get swap-throttled to the refresh rate
    %   of the operator desktop GUI display.
    Screen('Hookfunction', hmd{handle}.win, 'SetOneshotFlipFlags', '', kPsychDontAutoResetOneshotFlags + kPsychSkipWaitForFlipOnce + kPsychSkipVsyncForFlipOnce + kPsychSkipTimestampingForFlipOnce);
    hmd{handle}.debugDisplay = 1;
  else
    % Skip the OpenGL bufferswap for the onscreen window completely, ergo also skip
    % timestamping and allow timestamp injection from us instead:
    Screen('Hookfunction', hmd{handle}.win, 'SetOneshotFlipFlags', '', kPsychDontAutoResetOneshotFlags + kPsychSkipWaitForFlipOnce + kPsychSkipSwapForFlipOnce + kPsychSkipTimestampingForFlipOnce);
    hmd{handle}.debugDisplay = 0;
  end

  % Get size of onscreen window backbuffer for potential debug mirror blits:
  rect = Screen('GlobalRect', win);
  tw = RectWidth(rect);
  th = RectHeight(rect);

  if winfo.StereoMode > 0
    % In debug mode, setup mirror blit from left/right eye buffers to onscreen window OpenGL backbuffer:
    if hmd{handle}.debugDisplay
      copyString = sprintf('moglcore(''glBindFramebufferEXT'', 36009, 0); moglcore(''glBlitFramebufferEXT'', 0, 0, %i, %i, %i, 0, %i, %i, 16384, 9729);', width, height, 0, tw / 2, th);
      Screen('Hookfunction', win, 'AppendMFunction', 'LeftFinalizerBlitChain', 'OpenXR debug mirror blit left', copyString);
      Screen('HookFunction', win, 'Enable', 'LeftFinalizerBlitChain');
      copyString = sprintf('moglcore(''glBindFramebufferEXT'', 36009, 0); moglcore(''glBlitFramebufferEXT'', 0, 0, %i, %i, %i, 0, %i, %i, 16384, 9729);', width, height, tw / 2, tw, th);
      Screen('Hookfunction', win, 'AppendMFunction', 'RightFinalizerBlitChain', 'OpenXR debug mirror blit right', copyString);
      Screen('Hookfunction', win, 'Enable', 'RightFinalizerBlitChain');
    end
  else
    % In debug mode, setup mirror blit from mono buffer to onscreen window OpenGL backbuffer:
    if hmd{handle}.debugDisplay
      copyString = sprintf('moglcore(''glBindFramebufferEXT'', 36009, 0); moglcore(''glBlitFramebufferEXT'', 0, 0, %i, %i, 0, 0, %i, %i, 16384, 9729);', width, height, tw, th);
      Screen('Hookfunction', win, 'AppendMFunction', 'LeftFinalizerBlitChain', 'OpenXR debug mirror blit mono', copyString);
      Screen('Hookfunction', win, 'Enable', 'LeftFinalizerBlitChain');
    end
  end

  % Need to call the PsychOpenXR(1) callback at each Screen('Flip') to submit the output
  % frames to the XR-Compositor for presentation on the device. This gets called before an
  % OpenGL bufferswap (if any) + timestamping + validation will happen. It is supposed to
  % block until image presentation on the device has happened, and to inject proper Present
  % timestamps for 'Flip':
  % The kPsychSkipWaitForFlipOnce flag is set, so PreSwapbuffersOperations executes
  % immediately. We pass the tWhen timestamp to this fast-path callback, which will pass
  % it on to 'PresentFrame', and then on to the OpenXR runtime and OpenXR compositor to
  % present the image at the proper target time tWhen:
  cmdString = sprintf('PsychOpenXR(1, %i, IMAGINGPIPE_FLIPTWHEN);', handle);
  Screen('Hookfunction', win, 'AppendMFunction', 'PreSwapbuffersOperations', 'OpenXR Present Operation', cmdString);
  Screen('Hookfunction', win, 'Enable', 'PreSwapbuffersOperations');

  % Attach shutdown procedure on onscreen window close:
  cmdString = sprintf('PsychOpenXR(2, %i);', handle);
  Screen('Hookfunction', win, 'PrependMFunction', 'CloseOnscreenWindowPreGLShutdown', 'OpenXR cleanup', cmdString);
  Screen('Hookfunction', win, 'Enable', 'CloseOnscreenWindowPreGLShutdown');

  % Does usercode request auto-closing the device or driver when the onscreen window is closed?
  if hmd{handle}.autoclose > 0
    % Attach a window close callback for Device teardown at window close time:
    if hmd{handle}.autoclose == 2
      % Shutdown driver completely:
      Screen('Hookfunction', win, 'AppendMFunction', 'CloseOnscreenWindowPreGLShutdown', 'Shutdown window callback into PsychOpenXR driver.', 'PsychOpenXR(''Close'');');
    else
      % Only close this device:
      Screen('Hookfunction', win, 'PrependMFunction', 'CloseOnscreenWindowPreGLShutdown', 'Shutdown window callback into PsychOpenXR driver.', sprintf('PsychOpenXR(''Close'', %i);', handle));
    end
    Screen('HookFunction', win, 'Enable', 'CloseOnscreenWindowPreGLShutdown');
  end

  % Do "real" first connected controller query:
  hmd{handle}.controllerTypes = PsychOpenXRCore('Controllers', hmd{handle}.handle);

  % Query set of currently connected controllers:
  if strcmpi(hmd{handle}.subtype, 'Oculus') || strcmpi(hmd{handle}.subtype, 'SteamVR/OpenXR')
    % On at least the Oculus XR runtime on MS-Windows, and SteamVR on Windows and Linux,
    % we need this workaround, or 'Controllers' will not report connected controllers,
    % but only after multiple repeated queries. We don't want the 1st query
    % from a user-script to potentially fail, so lets do the failed queries
    % here already. Why? Nobody knows, but apparently somehow one needs to
    % sync up with the XR runtime, maybe in the xrWaitFrame() or
    % xrBeginFrame() calls inside Flip. Just calling 'Controllers' multiple
    % times did not help, despite that also executing xrSyncActions() and
    % XR event processing. Also the wait of at least this duration and
    % multiple repetitions are crucial. Just the normal nightmares of
    % dealing with proprietary runtimes...
    % SteamVR needs the extra 'Controllers' call below, or things will continue
    % to fail.
    for i = 1:3
      % Loop until we get a true flip through. This will spin-wait if device
      % is not detecting users presence, e.g., a VR device's proximity sensor
      % does not report "HMD firmly attached to users head". We break the
      % loop, once the user is ready.
      while Screen('Flip', win) == 0; end
      hmd{handle}.controllerTypes = PsychOpenXRCore('Controllers', hmd{handle}.handle);
      WaitSecs(0.5);
    end
  end

  % Set initial view layer type, depending if 3D perspective correct rendering
  % or pure 2D mono-/stereo drawing is used:
  if hmd{handle}.use3DMode
    % 3D: Default to projection layers:
    PsychOpenXRCore('ViewType', handle, 1);
  else
    % 2D: Default to quad view layers:
    PsychOpenXRCore('ViewType', handle, 0);
  end

  % Compute and assign default 2D quadView parameters:
  z = 1; % Default distance along optical axis / line of sight shall be 1 meter.
  za = z;

  % Forced override distance of 2D viewscreen provided?
  vOvrDist = strfind(hmd{handle}.basicRequirements, '2DViewDistMeters=');
  if ~isempty(vOvrDist)
    vOvrDist = sscanf(hmd{handle}.basicRequirements(min(vOvrDist):end), '2DViewDistMeters=%f');
    if ~isscalar(vOvrDist) || ~isnumeric(vOvrDist) || ~isreal(vOvrDist) || (vOvrDist <= 0)
      sca;
      error('Invalid ''2DViewDistMeters='' string in ''basicRequirements'' specified! Must be of the form ''2DViewDistMeters=2.1'' for example for 2.1 meters distance.');
    end

    % Override default distance: Values > 1 meter will shrink the view in
    % the field of view of the subject:
    za = vOvrDist;
  end

  if hmd{handle}.StereoMode > 0
    maxeye = 2;
  else
    maxeye = 1;
  end

  verbosefov = 0;
  if hmd{handle}.switchTo2DViewsOnStop || ~hmd{handle}.use3DMode
    verbosefov = 1;
  end

  % User override values for field of view specified in 'SetupRenderingParameters'?
  if ~isempty(hmd{handle}.fovL) && ~isempty(hmd{handle}.fovR)
    % Yes. Use userscript provided values:
    fov{1} = hmd{handle}.fovL;
    fov{2} = hmd{handle}.fovR;
    if verbosefov
      fprintf('PsychOpenXR-INFO: Using user-script provided identical field of view for both eyes for 2D view default parameter calculation.\n');
    end
  else
    % No. Need to find out ourselves:
    fov{1} = [];
    fov{2} = [];

    % Can we query values from OpenXR driver?
    if (hmd{myhmd.handle}.StereoMode > 0) || ~hmd{myhmd.handle}.noTrackingInMono
      % We can only query the XR device for field of view in stereo mode:
      [~, ~, fov{1}, fov{2}] = PsychOpenXRCore('GetStaticRenderParameters', handle);
    end

    % Results from driver now?
    if ~any(fov{1})
      % Nope, we have to make stuff up. Use the conservative field of view
      % settings from a Oculus Rift CV-1 when driven by the OculusXR runtime:
      fov{1} = [-0.6209, 0.6209, 0.7270, -0.8379];
      fov{2} = fov{1};
      if verbosefov
        fprintf('PsychOpenXR-INFO: No info about field of view available. Using hard-coded Oculus Rift CV-1 identical field of view for both eyes for 2D view default parameter calculation as a fallback.\n');
      end
    else
      if verbosefov
        fprintf('PsychOpenXR-INFO: Using OpenXR driver reported actual field of view for 2D view default parameter calculation.\n');
      end
    end
  end

  % Cache final fov values for use by 'GetStaticRenderParameters':
  hmd{handle}.fovL = fov{1};
  hmd{handle}.fovR = fov{2};

  for eye=1:maxeye
    % Get size as selected by driver to preserve square pixels for non-square window:
    [~, viewSize] = PsychOpenXRCore('View2DParameters', handle, eye - 1);

    % Aspect ratio:
    aspect = viewSize(1) / viewSize(2);

    % Use symmetric FoV calculations by default, as they give better results in practice:
    if isempty(strfind(hmd{handle}.basicRequirements, 'PerEyeFOV'))
      % Assume symmetric field of view. Rarely true in practice, but
      % gives much better results.

      % Total vertical field of view:
      % vfov = fov{eye}(3) - fov{eye}(4); %#ok<NASGU>

      % Minimal vertical field of view:
      mvfov = min(abs(fov{eye}(3:4)));

      % Total horizontal field of view:
      % hfov = fov{eye}(2) - fov{eye}(1); %#ok<NASGU>

      % Minimal horizontal field of view:
      % mhfov = 2 * min(abs(fov{eye}(1:2))); %#ok<NASGU>

      % Compute new vertical viewSize to fit into vertical field of view:
      viewSize(2) = z * 2 * tan(mvfov);

      % Compute matching horizontal viewSize for vertical viewSize, aspect ratio preserving:
      viewSize(1) = viewSize(2) * aspect;
      pos = [0, 0, -za];
      if verbosefov && eye == 1
        fprintf('PsychOpenXR-INFO: 2D view default setup uses minimum vertical field of view and aspect ratio for view size setup.\n');
      end
    else
      % Handle asymmetric field of view. Theoretically more correct, but
      % in practice much worse!

      % Width of components of asymetric horizontal field of view:
      wl = z * tan(abs(fov{eye}(1)));
      wr = z * tan(abs(fov{eye}(2)));

      % Total width:
      w = wl + wr;

      % Corrective x-shift to compensate for asymetry:
      x = (wl - wr) / 2;

      % Height of components of asymetric vertical field of view:
      hu = z * tan(abs(fov{eye}(3)));
      hd = z * tan(abs(fov{eye}(4)));

      % Total height:
      h = hu + hd;

      % Corrective x-shift to compensate for asymetry:
      y = (hu - hd) / 2;

      viewSize = [w, h];
      pos = [x, y, -za];
      if verbosefov && eye == 1
        fprintf('PsychOpenXR-INFO: 2D view default setup uses per-eye (asymmetrical) field of view for view size and position setup.\n');
      end
    end

    PsychOpenXRCore('View2DParameters', handle, eye - 1, pos, viewSize);
  end

  % Eye tracking wanted in this session and supported by system?
  if hmd{handle}.needEyeTracking && hmd{handle}.eyeTrackingSupported
    % Tracking api specific setup:

    % SRAnipalMex available for HTC Vive SRAnipal eye tracking?
    if IsWin && exist('SRAnipalMex', 'file') && ~isempty(strfind(hmd{handle}.modelName, 'SRanipal'))
      % Yes. Use this instead of standard PsychOpenXRCore provided eye tracking OpenXR extensions:
      fprintf('PsychOpenXR-INFO: Trying to enable HTC SRAnipal eye tracking for this session.\n');

      % Perform an eye gaze tracking query via OpenXR extensions. We don't
      % care about the actual result, but on HTC devices like this one,
      % this will force-load/link the SRanipal.dll which is contained /
      % bundled within HTC's installed driver software for Vive devices
      % into the Matlab/Octave process, as HTC's OpenXR gaze tracking
      % extension is just a thin wrapper around HTC's SRanipal api and
      % runtime, specifically their XR_EXT_eye_gaze_interaction extension
      % simply calls SRanipal's GetEyeData_v2() function, then translates
      % and returns the data in OpenXR format. As our SRAnipalMex file also
      % has a load-time dependency on SRanipal.dll, this should allow our
      % mex file to load and link without trouble - or so goes the theory:
      PsychOpenXRCore('GetTrackingState', handle, [], 4);

      % Initialize eyetracker connection:
      try
        if SRAnipalMex(0)
          % Start data acquisition:
          SRAnipalMex(2);

          % Upgrade eyeTrackingSupported:
          % +1 "Basic" monocular/single gazevector
          % +2 Binocular/separate left/right eye gaze
          % +1024 HTC SRAnipal eye tracking in use
          hmd{handle}.eyeTrackingSupported = 1 + 2 + 1024;
        else
          warning('HTC SRAnipal eye tracker startup failed! Trying more limited standard OpenXR eye tracking instead.');
        end
      catch
        fprintf('PsychOpenXR-INFO: HTC SRAnipal runtime interface DLL unavailable. Trying more limited standard OpenXR eye tracking instead.\n');
      end
    end
  end

  % Tracked operation requested?
  if ~isempty(strfind(hmd{handle}.basicTask, 'Tracked'))
    % 3D head tracked VR rendering task: Start tracking as a convenience:
    PsychOpenXRCore('Start', handle);
  end

  % Last step: Start presenter thread if always-on multi-threading is requested:
  if hmd{handle}.multiThreaded == 2
    if hmd{handle}.needWinThreadingWa1
      Screen('Hookfunction', hmd{handle}.win, 'SetDisplayBufferTextures', '', hmd{handle}.oldglLeftTex, hmd{handle}.oldglRightTex);
    end

    PsychOpenXRCore('PresenterThreadEnable', handle, 1);
  end

  % Return success result code 1:
  varargout{1} = 1;
  return;
end

% 'cmd' so far not dispatched? Let's assume it is a command
% meant for PsychOpenXRCore:
if (length(varargin) >= 1) && isstruct(varargin{1})
  myhmd = varargin{1};
  handle = myhmd.handle;
  [ varargout{1:nargout} ] = PsychOpenXRCore(cmd, handle, varargin{2:end});
else
  [ varargout{1:nargout} ] = PsychOpenXRCore(cmd, varargin{:});
end

return;

end
