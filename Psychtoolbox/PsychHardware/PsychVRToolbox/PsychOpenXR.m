function varargout = PsychOpenXR(cmd, varargin)
% PsychOpenXR - A high level driver for OpenXR supported XR hardware.
%
% Copyright (c) 2022-2023 Mario Kleiner. Licensed to you under the MIT license.
% Our underlying PsychOpenXRCore mex driver builds against the Khronos OpenXR SDK public
% headers, and links against the OpenXR open-source dynamic loader, to implement the
% interface to a system-installed OpenXR runtime. These components are dual-licensed by
% Khronos under Apache 2.0 and MIT license: SPDX license identifier "Apache-2.0 OR MIT"
%
% Note: If you want to write VR code that is portable across
% VR headsets of different vendors, then use the PsychVRHMD()
% driver instead of this driver. The PsychVRHMD driver will use
% this driver as appropriate when connecting to a OpenXR supported XR device,
% but it will also automatically work
% with other head mounted displays. This driver does however
% expose a few functions specific to OpenXR hardware, so you can
% mix calls to this driver with calls to PsychVRHMD to do some
% mix & match.
%
% For setup instructions for OpenXR, see "help OpenXR". TODO
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
% - Open a OpenXR HMD, set it up with good default rendering and
% display parameters and generate a PsychImaging('AddTask', ...)
% line to setup the Psychtoolbox imaging pipeline for proper display
% on the HMD. This will also cause the device connection to get
% auto-closed as soon as the onscreen window which displays on
% the HMD is closed. Returns the 'hmd' handle of the HMD on success.
%
% By default, the first detected HMD will be used and if no VR HMD
% is connected, it will return an empty [] hmd handle. You can override
% this default choice of HMD by specifying the optional 'deviceIndex'
% parameter to choose a specific HMD. However, only one HMD per machine is
% supported, so the 'deviceIndex' will probably be only useful in the future.
%
% More optional parameters: 'basicTask' what kind of task should be implemented.
% The default is 'Tracked3DVR', which means to setup for stereoscopic 3D
% rendering, driven by head motion tracking, for a fully immersive experience
% in some kind of 3D virtual world. This is the default if omitted. The task
% 'Stereoscopic' sets up for display of stereoscopic stimuli, but without
% head tracking. 'Monoscopic' sets up for display of monocular stimuli, ie.
% the HMD is just used as a special kind of standard display monitor. In 'Monoscopic'
% and 'Stereoscopic' mode, both eyes will be presented with an identical field of view,
% to make sure pure 2D drawing works, without the need for setup of special per-eye
% projection transformations. In 'Tracked3DVR' mode, each eye will have a different
% field of view, optimized to maximize the viewable area while still avoiding occlusion
% artifacts due to the nose of the wearer of the HMD.
%
% 'basicRequirements' defines basic requirements for the task. Currently
% defined are the following strings which can be combined into a single
% 'basicRequirements' string:
%
% 'Float16Display' = Request rendering, compositing and display in 16 bpc float
% format. This will ask Psychtoolbox to render and post-process stimuli in 16 bpc
% linear floating point format, and allocate 16 bpc half-float textures as final
% renderbuffers to be sent to the VR compositor. If the VR compositor takes advantage
% of the high source image precision is at the discretion of the compositor and HMD.
% By default, if this request is omitted, processing and display in sRGB format is
% requested from Psychtoolbox and the compositor, ie., a roughly gamma 2.2 8 bpc
% format is used.
%
% 'TimingSupport' = Support some hardware specific means of timestamping
% or latency measurements. On the Rift DK1 this does nothing. On the DK2
% it enables dynamic prediction and timing measurements with the Rifts internal
% latency tester.
%
% 'basicQuality' defines the basic tradeoff between quality and required
% computational power. A setting of 0 gives lowest quality, but with the
% lowest performance requirements. A setting of 1 gives maximum quality at
% maximum computational load. Values between 0 and 1 change the quality to
% performance tradeoff.
%
%
% hmd = PsychOpenXR('Open' [, deviceIndex], ...);
% - Open HMD with index 'deviceIndex'. See PsychOpenXRCore Open?
% for help on additional parameters.
%
%
% PsychOpenXR('SetAutoClose', hmd, mode);
% - Set autoclose mode for HMD with handle 'hmd'. 'mode' can be
% 0 (this is the default) to not do anything special. 1 will close
% the HMD 'hmd' when the onscreen window is closed which displays
% on the HMD. 2 will do the same as 1, but close all open HMDs and
% shutdown the complete driver and OpenXR runtime - a full cleanup.
%
%
% isOpen = PsychOpenXR('IsOpen', hmd);
% - Returns 1 if 'hmd' corresponds to an open HMD, 0 otherwise.
%
%
% PsychOpenXR('Close' [, hmd]);
% - Close provided HMD device 'hmd'. If no 'hmd' handle is provided,
% all HMDs will be closed and the driver will be shutdown.
%
%
% PsychOpenXR('Controllers', hmd);
% - Return a bitmask of all connected controllers: Can be the bitand
% of the OVR.ControllerType_XXX flags described in 'GetInputState'.
%
%
% info = PsychOpenXR('GetInfo', hmd);
% - Retrieve a struct 'info' with information about the HMD 'hmd'.
% The returned info struct contains at least the following standardized
% fields with information:
%
% handle = Driver internal handle for the specific HMD.
% driver = Function handle to the actual driver for the HMD, e.g., @PsychOpenXR.
% type   = Defines the type/vendor of the device, e.g., 'OpenXR'.
% modelName = Name string with the name of the model of the device, e.g., 'Rift DK2'.
% separateEyePosesSupported = 1 if use of PsychOpenXR('GetEyePose') will improve
%                             the quality of the VR experience, 0 if no improvement
%                             is to be expected, so 'GetEyePose' can be avoided
%                             to save processing time without a loss of quality.
%                             This *always* returns 0 on this PsychOpenXR driver.
%
% The returned struct may contain more information, but the fields mentioned
% above are the only ones guaranteed to be available over the long run. Other
% fields may disappear or change their format and meaning anytime without
% warning.
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
% current extents. This is not supported by OpenXR, therefore the function does
% nothing, but return backwards compatible dummy values.
%
% 'requestVisible' 1 = Request showing the boundary area markers, 0 = Don't
% request showing the markers. This parameter is accepted, but ignored.
%
% Returns in 'isVisible' the current visibility status of the VR area boundaries.
% This driver always returns 0 for false / invisible.
%
% 'playAreaBounds' is a 3-by-n matrix defining the play area boundaries. Each
% column represents the [x;y;z] coordinates of one 3D definition point. Connecting
% successive points by line segments defines the boundary, as projected onto the
% floor. Points are listed in clock-wise direction. An empty return argument means
% that the play area is so far undefined. This driver always returns empty.
%
% 'OuterAreaBounds' defines the outer area boundaries in the same way as
% 'playAreaBounds'. This driver always returns empty.
%
%
% input = PsychOpenXR('GetInputState', hmd, controllerType);
% - Get input state of controller 'controllerType' associated with HMD 'hmd'.
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
% OVR.ControllerType_Remote - Connected remote control or similar, e.g., control buttons on HMD.
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
% +1  = Our rendering goes to the HMD, ie. we have control over it. Lack of this could
%       mean the Health and Safety warning is displaying at the moment and waiting for
%       acknowledgement, or the OpenXR GUI application is in control.
% +2  = HMD is present and active.
% +4  = HMD is strapped onto users head. A Rift CV1 would switch off/blank if not on the head.
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
%      contained in state.cameraView{1} and {2}.
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
% 0 = Origin is at eye height (HMD height).
% 1 = Origin is at floor height.
% The eye height or floor height gets defined by the system during
% sensor calibration, possibly guided by some OpenXR GUI control application.
%
%
% PsychOpenXR('SetupRenderingParameters', hmd [, basicTask='Tracked3DVR'][, basicRequirements][, basicQuality=0][, fov=[HMDRecommended]][, pixelsPerDisplay=1])
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
% user. Note: This parameter is completely ignored with the current driver and on
% a standard OpenXR 1.0 backend.
%
% 'pixelsPerDisplay' Ratio of the number of render target pixels to display pixels
% at the center of distortion. Defaults to 1.0 if omitted. Lower values can
% improve performance, at lower quality. Note: This parameter is completely ignored
% with the current driver and on a standard OpenXR 1.0 backend.
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
% - Return old settings for panel overdrive mode in 'oldSettings',
% optionally set new settings in 'newparams'. This changes the operating
% parameters of OLED panel overdrive on the Rift DK-2 if 'FastResponse'
% mode is active. newparams is a vector [upscale, downscale, gamma] with
% the following meaning: gamma = 1 Use gamma/degamma pass to perform
% overdrive boost in gamma 2.2 corrected space. This is the startup default.
% upscale = How much should rising pixel color intensity values be boosted.
% Default is 0.10 for a 10% boost.
% downscale = How much should rising pixel color intensity values be reduced.
% Default is 0.05 for a 5% reduction.
% The Rift DK-2 OLED panel controller is slower on rising intensities than on
% falling intensities, therefore the higher boost on rising than on falling
% direction.
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
% renderbuffer for each eye for rendering to the HMD. Returns parameters
% previously computed by PsychOpenXR('SetupRenderingParameters', hmd).
%
% Also returns 'imagingFlags', the required imaging mode flags for setup of
% the Screen imaging pipeline. Also returns the needed 'stereoMode' for the
% pipeline.
%
%
% needPanelFitter = PsychOpenXR('GetPanelFitterParameters', hmd);
% - 'needPanelFitter' is 1 if a custom panel fitter tasks is needed, and 'bufferSize'
% from the PsychVRHMD('GetClientRenderingParameters', hmd); defines the size of the
% clientRect for the onscreen window. 'needPanelFitter' is 0 if no panel fitter is
% needed.
%
%
% [winRect, ovrfbOverrideRect, ovrSpecialFlags, ovrMultiSample] = PsychOpenXR('OpenWindowSetup', hmd, screenid, winRect, ovrfbOverrideRect, ovrSpecialFlags, ovrMultiSample);
% - Compute special override parameters for given input/output arguments, as needed
% for a specific HMD. Take other preparatory steps as needed, immediately before the
% Screen('OpenWindow') command executes. This is called as part of PsychImaging('OpenWindow'),
% with the user provided hmd, screenid, winRect etc.
%
%
% isOutput = PsychOpenXR('IsHMDOutput', hmd, scanout);
% - Returns 1 (true) if 'scanout' describes the video output to which the
% HMD 'hmd' is connected. 'scanout' is a struct returned by the Screen
% function Screen('ConfigureDisplay', 'Scanout', screenid, outputid);
% This allows probing video outputs to find the one which feeds the HMD.
% Deprecated: This function does nothing. It just exists for (backwards)
% compatibility with PsychVRHMD.
%
%

% Global GL handle for access to OpenGL constants needed in setup:
global GL;
global OVR;

persistent firsttime;
persistent oldShieldingLevel;
persistent hmd;

if nargin < 1 || isempty(cmd)
  help PsychOpenXR;
  fprintf('\n\nAlso available are functions from PsychOpenXRCore:\n');
  PsychOpenXRCore;
  return;
end

% Fast-Path function 'EndFrameRender' - Queues new frames to Compositor:
if cmd == 0
  % Submit/Commit just unbound textures to texture swap-chains:
  PsychOpenXRCore('EndFrameRender', hmd{varargin{1}}.handle);

  if hmd{varargin{1}}.steamXROpenGLWa
    % SteamVR leaves our OpenGL context in a disabled state after
    % 'EndFrameRender' aka xrReleaseSwapchainImages(), which would
    % cause OpenGL errors. The following 'GetWindowInfo' forces our
    % OpenGL context back on to resolve the problem:
    Screen('GetWindowInfo', hmd{varargin{1}}.win);
  end

  return;
end

% Fast-Path function 'PresentFrame' - Present frame to VR compositor,
% wait for present completion, inject present completion timestamps:
if cmd == 1
  handle = varargin{1};
  tWhen = varargin{2};

  % Present and timestamp:
  [predictedOnset, hmd{handle}.predictedFutureOnset, hmd{handle}.debugFlipTime] = PsychOpenXRCore('PresentFrame', hmd{handle}.handle, tWhen);
  %[predictedOnset, hmd{handle}.predictedFutureOnset, hmd{handle}.debugFlipTime] = PsychOpenXRCore('PresentFrame', hmd{handle}.handle, tWhen);
  %predictedOnset = hmd{handle}.predictedFutureOnset; % Testing on MonadoXR shows this is the right(est) timestamp, iow. without subtraction.

  % Assign return values for vblTime and stimulusOnsetTime for Screen('Flip'):
  Screen('Hookfunction', hmd{handle}.win, 'SetOneshotFlipResults', '', predictedOnset, predictedOnset);

  % PresentFrame successfull and not skipped?
  if predictedOnset > 0
    % Get fresh set of backing textures for next Screen() post-flip drawing/render
    % cycle from the OpenXR texture swap chains:
    texLeft = PsychOpenXRCore('GetNextTextureHandle', hmd{handle}.handle, 0);
    if hmd{handle}.StereoMode > 0
      texRight = PsychOpenXRCore('GetNextTextureHandle', hmd{handle}.handle, 1);
    else
      texRight = [];
    end

    % Attach them as new backing textures, detach the previously bound ones, so they
    % are ready for submission to the VR compositor:
    Screen('Hookfunction', hmd{handle}.win, 'SetDisplayBufferTextures', '', texLeft, texRight);
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

if strcmpi(cmd, 'PrepareRender')
  % Get and validate handle - fast path open coded:
  myhmd = varargin{1};
  if ~((length(hmd) >= myhmd.handle) && (myhmd.handle > 0) && hmd{myhmd.handle}.open)
    error('PsychOpenXR:PrepareRender: Specified handle does not correspond to an open HMD!');
  end

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

  % Mark start of a new frame render cycle for the runtime:
  PsychOpenXRCore('StartRender', myhmd.handle, targetTime);

  % Get predicted eye pose, tracking state and hand controller poses (if supported) for targetTime:
  [state, touch] = PsychOpenXRCore('GetTrackingState', myhmd.handle, targetTime);
  myhmd.state = state;

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
    % right eye. For HMD's without gaze tracking, this is a reasonable approximation, as
    % they track HMD position and derive eye pose from HMD pose, so we just undo that. For
    % a HMD with gaze tracking that would use gaze info to compute different eye orientation
    % for each eye, this would go wrong, and something more clever would be needed, to at
    % least get a roughly correct approximation of HMD orientation, although an exactly
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

  varargout{1} = result;

  return;
end

if strcmpi(cmd, 'GetEyePose')
  % Get and validate handle - fast path open coded:
  myhmd = varargin{1};
  if ~((length(hmd) >= myhmd.handle) && (myhmd.handle > 0) && hmd{myhmd.handle}.open)
    error('PsychOpenXR:GetEyePose: Specified handle does not correspond to an open HMD!');
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
  eyes = PsychOpenXRCore('GetTrackingState', myhmd.handle, targetTime);

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
%    error('PsychOpenXR:GetTrackersState: Specified handle does not correspond to an open HMD!');
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
    error('PsychOpenXR:GetInputState: Specified handle does not correspond to an open HMD!');
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
    error('PsychOpenXR:HapticPulse: Specified handle does not correspond to an open HMD!');
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
    error('PsychOpenXR:Start: Specified handle does not correspond to an open HMD!');
  end

  % Use of multi-threading only in stopped 3D mode?
  if hmd{myhmd.handle}.multiThreaded == 1
    % Stop thread:
    PsychOpenXRCore('PresenterThreadEnable', hmd{myhmd.handle}.handle, 0);
  end

  % Mark userscript driven tracking as active:
  PsychOpenXRCore('Start', hmd{myhmd.handle}.handle);

  return;
end

if strcmpi(cmd, 'Stop')
  % Get and validate handle - fast path open coded:
  myhmd = varargin{1};
  if ~((length(hmd) >= myhmd.handle) && (myhmd.handle > 0) && hmd{myhmd.handle}.open)
    error('PsychOpenXR:Stop: Specified handle does not correspond to an open HMD!');
  end

  % Use of multi-threading only needed in stopped 3D mode?
  if (hmd{myhmd.handle}.multiThreaded == 1) && hmd{myhmd.handle}.use3DMode && ...
     PsychOpenXRCore('NeedLocateForProjectionLayers', hmd{myhmd.handle}.handle)
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
    error('VRAreaBoundary: Passed in handle does not refer to a valid and open HMD.');
  end

  % Return no-op values for this unsupported, but mandated by PsychVRHMD(), function:
  [varargout{1}, varargout{2}, varargout{3}] = deal(0, [], []);
  return;
end

if strcmpi(cmd, 'TrackingOriginType')
  myhmd = varargin{1};
  if ~((length(hmd) >= myhmd.handle) && (myhmd.handle > 0) && hmd{myhmd.handle}.open)
    error('PsychOpenXR:TrackingOriginType: Specified handle does not correspond to an open HMD!');
  end

  varargout{1} = PsychOpenXRCore('TrackingOriginType', myhmd.handle, varargin{2:end});

  return;
end

if strcmpi(cmd, 'Supported')
  % Check if the OpenXR runtime 1+ is supported and active on this
  % installation, so it can be used to open connections to real HMDs,
  % or at least to emulate a HMD for simple debugging purposes:
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

% Autodetect first connected HMD and open a connection to it. Open a
% emulated one, if none can be detected. Perform basic setup with
% default configuration, create a proper PsychImaging task.
if strcmpi(cmd, 'AutoSetupHMD')
  % Do we have basic runtime support?
  if ~PsychOpenXR('Supported')
    % Nope: Game over.
    fprintf('PsychOpenXR:AutoSetupHMD: Could not initialize OpenXR driver. Game over!\n');

    % Return an empty handle to signal lack of VR HMD support to caller,
    % so caller can cope with it somehow:
    varargout{1} = [];
    return;
  end

  % Basic task this HMD should fulfill:
  if length(varargin) >= 1 && ~isempty(varargin{1})
    basicTask = varargin{1};
  else
    basicTask = 'Tracked3DVR';
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
    basicQuality = 0;
  end

  % HMD device selection:
  if length(varargin) >= 4 && ~isempty(varargin{4})
    deviceIndex = varargin{4};
    newhmd = PsychOpenXR('Open', deviceIndex);
  else
    % Check if at least one OpenXR HMD is connected and available:
    if PsychOpenXR('GetCount') > 0
      % Yes. Open and initialize connection to first detected HMD:
      fprintf('PsychOpenXR: Opening the first connected OpenXR VR headset.\n');
      newhmd = PsychOpenXR('Open', 0);
    else
      % HMD emulation not possible:
      fprintf('PsychOpenXR: No OpenXR HMD detected. Game over.\n');
      varargout{1} = [];
      return;
    end
  end

  % Trigger an automatic device close at onscreen window close for the HMD display window:
  PsychOpenXR('SetAutoClose', newhmd, 1);

  % Setup default rendering parameters:
  PsychOpenXR('SetupRenderingParameters', newhmd, basicTask, basicRequirements, basicQuality);

  % Add a PsychImaging task to use this HMD with the next opened onscreen window:
  PsychImaging('AddTask', 'General', 'UseVRHMD', newhmd);

  % Return the device handle:
  varargout{1} = newhmd;

  % Ready.
  return;
end

if strcmpi(cmd, 'SetAutoClose')
  myhmd = varargin{1};

  if ~PsychOpenXR('IsOpen', myhmd)
    error('PsychOpenXR:SetAutoClose: Specified handle does not correspond to an open HMD!');
  end

  % Assign autoclose flag:
  hmd{myhmd.handle}.autoclose = varargin{2};

  return;
end

if strcmpi(cmd, 'SetHSWDisplayDismiss')
  myhmd = varargin{1};

  if ~PsychOpenXR('IsOpen', myhmd)
    error('PsychOpenXR:SetHSWDisplay: Specified handle does not correspond to an open HMD!');
  end

  % Method of dismissing HSW display:
  if length(varargin) < 2 || isempty(varargin{2})
    % Default is keyboard, mouse click, or HMD tap:
    hmd{myhmd.handle}.hswdismiss = 1 + 2 + 4;
  else
    hmd{myhmd.handle}.hswdismiss = varargin{2};
  end

  return;
end

% Open a HMD:
if strcmpi(cmd, 'Open')
  if isempty(firsttime)
    firsttime = 1;
    fprintf('Copyright (c) 2022-2023 Mario Kleiner. Licensed to you under the MIT license.\n');
    fprintf('Our underlying PsychOpenXRCore mex driver builds against the Khronos OpenXR SDK public\n');
    fprintf('headers, and links against the OpenXR open-source dynamic loader, to implement the\n');
    fprintf('interface to a system-installed OpenXR runtime. These components are dual-licensed by\n');
    fprintf('Khronos under Apache 2.0 and MIT license: SPDX license identifier “Apache-2.0 OR MIT”\n\n');
  end

  [handle, modelName, runtimeName] = PsychOpenXRCore('Open', varargin{:});

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

  % Default to multiThreaded off:
  newhmd.multiThreaded = 0;

  % SteamVR OpenXR runtime needs a workaround for not properly
  % managing its OpenGL context sometimes. So far confirmed to be
  % needed on Linux with SteamVR 1.24.6. Status on Windows not yet known:
  if IsLinux && strcmp(runtimeName, 'SteamVR/OpenXR')
    newhmd.steamXROpenGLWa = 1;
  else
    newhmd.steamXROpenGLWa = 0;
  end

  % Monado OpenXR runtime does not need frequent tracking to keep
  % projection layers stable and free of jitter/jerk/timeout warnings.
  if ~isempty(strfind(runtimeName, 'Monado')) %#ok<STREMP>
    % Monado or similar advanced: No need for this - Shaves off some
    % millisecond from a multi-threaded / not client-tracked loop and gives
    % extra visual stability:
    PsychOpenXRCore('NeedLocateForProjectionLayers', handle, 0);
  else
    % Less advanced: Need tracking update, and multi-threading if the
    % client does not use active fast tracking:
    PsychOpenXRCore('NeedLocateForProjectionLayers', handle, 1);
    newhmd.multiThreaded = 1;
  end

  % Default autoclose flag to "no autoclose":
  newhmd.autoclose = 0;

  % By default allow user to dismiss HSW display via key press,
  % mouse click, or HMD tap:
  newhmd.hswdismiss = 1 + 2 + 4;

  % Setup basic task/requirement/quality specs to "nothing":
  newhmd.basicQuality = 0;
  newhmd.basicTask = '';
  newhmd.basicRequirements = '';

  % Start with invalid future timestamps or debug timestamp:
  newhmd.predictedFutureOnset = NaN;
  newhmd.debugFlipTime = 0;

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
    error('Controllers: Passed in handle does not refer to a valid and open HMD.');
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
    error('GetInfo: Passed in handle does not refer to a valid and open HMD.');
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

  % TODO FIXME: Any special handling for TimingSupport or Tracked3DVR ?
  if ~isempty(strfind(hmd{handle}.basicRequirements, 'TimingSupport')) || ...
     ~isempty(strfind(hmd{handle}.basicTask, 'Tracked3DVR'))
    %    PsychOpenXRCore('SetDynamicPrediction', handle, 1);
  else
    %    PsychOpenXRCore('SetDynamicPrediction', handle, 0);
  end

  return;
end

if strcmpi(cmd, 'SetFastResponse')
  myhmd = varargin{1};
  if ~PsychOpenXR('IsOpen', myhmd)
    error('SetFastResponse: Passed in handle does not refer to a valid and open HMD.');
  end
  handle = myhmd.handle;

  % FastResponse has no meaningful implementation on the OpenXR runtime, so just
  % return a constant old value of 1 for "fast response always enabled":
  varargout{1} = 1;

  return;
end

if strcmpi(cmd, 'SetTimeWarp')
  myhmd = varargin{1};
  if ~PsychOpenXR('IsOpen', myhmd)
    error('SetTimeWarp: Passed in handle does not refer to a valid and open HMD.');
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
    error('SetLowPersistence: Passed in handle does not refer to a valid and open HMD.');
  end

  % SetLowPersistence defined the use of low persistence mode on the Rift DK2 with
  % the OculusVR v0.5 SDK and the original PsychOculusVR driver. We don't have control
  % over this on OpenXR, so for backwards compatibility, always return constant old
  % setting "Always low persistence":
  varargout{1} = 1;

  return;
end

if strcmpi(cmd, 'SetupRenderingParameters')
  myhmd = varargin{1};

  % Basic task this HMD should fulfill:
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

  hmd{myhmd.handle}.basicTask = basicTask;
  hmd{myhmd.handle}.basicRequirements = basicRequirements;

  PsychOpenXR('SetBasicQuality', myhmd, basicQuality);

  % Get optimal client renderbuffer size - the size of our virtual framebuffer for left eye:
  [hmd{myhmd.handle}.rbwidth, hmd{myhmd.handle}.rbheight, hmd{myhmd.handle}.recMSAASamples, hmd{myhmd.handle}.maxMSAASamples] = PsychOpenXRCore('GetFovTextureSize', myhmd.handle, 0);

  % Get optimal client renderbuffer size - the size of our virtual framebuffer for right eye:
  [hmd{myhmd.handle}.rbwidth, hmd{myhmd.handle}.rbheight, hmd{myhmd.handle}.recMSAASamples, hmd{myhmd.handle}.maxMSAASamples] = PsychOpenXRCore('GetFovTextureSize', myhmd.handle, 1);

  % Debug display of HMD output into onscreen window requested?
  if isempty(strfind(basicRequirements, 'DebugDisplay')) && isempty(oldShieldingLevel)
    % No. Set to be created onscreen window to be invisible:
    oldShieldingLevel = Screen('Preference', 'WindowShieldingLevel', -1);
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
  % 'DebugDisplay' mode for mirroring of HMD content to the onscreen window
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

% [winRect, ovrfbOverrideRect, ovrSpecialFlags, ovrMultiSample] = PsychOpenXR('OpenWindowSetup', hmd, screenid, winRect, ovrfbOverrideRect, ovrSpecialFlags, ovrMultiSample);
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

  % As the onscreen window is not used for displaying on the HMD, but
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

  fprintf('PsychOpenXR-Info: Overriding onscreen window framebuffer size to %i x %i pixels for use with VR-HMD direct output mode.\n', ...
          clientRes(1), clientRes(2));

  % Skip all visual timing sync tests and calibrations, as display timing
  % of the onscreen window doesn't matter, only the timing on the HMD direct
  % output matters - and that can't be measured by our standard procedures:
  Screen('Preference', 'SkipSyncTests', 2);

  varargout{1} = winRect;
  varargout{2} = ovrfbOverrideRect;
  varargout{3} = ovrSpecialFlags;
  varargout{4} = ovrMultiSample;

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

  % Create and startup XR session, based on the Screen() OpenGL interop info in 'gli':
  gli = Screen('GetWindowInfo', win, 9);

  % Multithreaded operation, with a separate OpenXR frame worker thread
  % inside PsychOpenXRCore? This would need a dedicated interop OpenGL
  % context assigned to that thread only:
  if hmd{handle}.multiThreaded
    % Use dedicated OpenGL context for OpenXR worker thread:
    openglContext = gli.OpenGLContext;
  else
    % Use Screen()'s main OpenGL context for everything, both Screen and OpenXR OpenGL ops:
    openglContext = gli.OpenGLContextScreen;
  end

  % Query currently bound finalizedFBO backing textures, to keep them around as backups for restoration when closing down the session:
  [hmd{handle}.oldglLeftTex, hmd{handle}.oldglRightTex, textarget, texformat, texmultisample, texwidth, texheight, fboIds(1), fboIds(2)] = Screen('Hookfunction', win, 'GetDisplayBufferTextures'); %#ok<NASGU> 

  % Override fboIds with backing texture handles in production use:
  if 1
    fboIds = [hmd{handle}.oldglLeftTex, hmd{handle}.oldglRightTex];
  else
    % This triggers the slower FBO based glCopyTexSubImage2D() path, which
    % can only handle non-MSAA content:
    fboIds = -fboIds; %#ok<UNRCH>
    warning('Slow MS-Windows fallback for copies triggered, which also can not handle native OpenXR MSAA.');
  end

  % Create and start OpenXR session:
  [hmd{handle}.videoRefreshDuration] = PsychOpenXRCore('CreateAndStartSession', hmd{handle}.handle, gli.DeviceContext, openglContext, gli.OpenGLDrawable, ...
                                                                                gli.OpenGLConfig, gli.OpenGLVisualId, hmd{handle}.use3DMode, hmd{handle}.multiThreaded, fboIds);

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
  [width, height, numTextures, texChainFormat] = PsychOpenXRCore('CreateRenderTextureChain', hmd{handle}.handle, 0, hmd{handle}.rbwidth, hmd{handle}.rbheight, floatFlag, hmd{handle}.texmultisample);

  % Create 2nd chain for right eye in stereo mode:
  if winfo.StereoMode > 0
    if winfo.StereoMode ~=12
      sca;
      error('Invalid Screen() StereoMode in use for OpenXR! Must be mode 12.');
    end
    [width, height, numTextures, texChainFormat] = PsychOpenXRCore('CreateRenderTextureChain', hmd{handle}.handle, 1, hmd{handle}.rbwidth, hmd{handle}.rbheight, floatFlag, hmd{handle}.texmultisample);
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
  Screen('Hookfunction', win, 'SetDisplayBufferTextures', '', texLeft, texRight, [], texChainFormat);

  % Go back to user requested clear color, now that all our buffers
  % are cleared to black:
  Screen('FillRect', win, clearcolor);

  % Define parameters for the ongoing Psychtoolbox onscreen window flip operation:
  % Debug display of HMD output into onscreen window requested?
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

  % Need to call the PsychOpenXR(0) callback at each Screen('Flip') to get the imaging
  % pipelines post-processed final output frames for left/right eye and commit them to
  % the XR-Compositors texture swap-chain(s), then setting up new target textures as
  % buffers for next cycle. This happens at the end of preflip operations, as part of
  % the implicit 'DrawingFinished':
  cmdString = sprintf('PsychOpenXR(0, %i, IMAGINGPIPE_FLIPTWHEN, IMAGINGPIPE_FLIPVBLSYNCLEVEL);', handle);
  if winfo.StereoMode > 0
    % In debug mode, setup mirror blit from left/right eye buffers to onscreen window OpenGL backbuffer:
    if hmd{handle}.debugDisplay
      copyString = sprintf('moglcore(''glBindFramebufferEXT'', 36009, 0); moglcore(''glBlitFramebufferEXT'', 0, 0, %i, %i, %i, 0, %i, %i, 16384, 9729);', width, height, 0, tw / 2, th);
      Screen('Hookfunction', win, 'AppendMFunction', 'LeftFinalizerBlitChain', 'OpenXR debug mirror blit left', copyString);
      Screen('HookFunction', win, 'Enable', 'LeftFinalizerBlitChain');
      copyString = sprintf('moglcore(''glBindFramebufferEXT'', 36009, 0); moglcore(''glBlitFramebufferEXT'', 0, 0, %i, %i, %i, 0, %i, %i, 16384, 9729);', width, height, tw / 2, tw, th);
      Screen('Hookfunction', win, 'AppendMFunction', 'RightFinalizerBlitChain', 'OpenXR debug mirror blit right', copyString);
    end

    % In stereo mode, use right finalizer chain for right texture detach and both textures commit, as it executes last:
    Screen('Hookfunction', win, 'AppendMFunction', 'RightFinalizerBlitChain', 'OpenXR Stereo commit Operation', cmdString);
    Screen('Hookfunction', win, 'Enable', 'RightFinalizerBlitChain');
  else
    % In debug mode, setup mirror blit from mono buffer to onscreen window OpenGL backbuffer:
    if hmd{handle}.debugDisplay
      copyString = sprintf('moglcore(''glBindFramebufferEXT'', 36009, 0); moglcore(''glBlitFramebufferEXT'', 0, 0, %i, %i, 0, 0, %i, %i, 16384, 9729);', width, height, tw, th);
      Screen('Hookfunction', win, 'AppendMFunction', 'LeftFinalizerBlitChain', 'OpenXR debug mirror blit mono', copyString);
    end

    % In mono mode, use left finalizer chain for texture detach and commit, as it executes only - and therefore last:
    Screen('Hookfunction', win, 'AppendMFunction', 'LeftFinalizerBlitChain', 'OpenXR Mono commit Operation', cmdString);
    Screen('Hookfunction', win, 'Enable', 'LeftFinalizerBlitChain');
  end

  % Need to call the PsychOpenXR(1) callback at each Screen('Flip') to submit the output
  % frames to the VR-Compositor for presentation on the HMD. This gets called before an
  % OpenGL bufferswap (if any) + timestamping + validation will happen. It is supposed to
  % block until image presentation on the HMD has happened, and to inject proper Present
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

  % Does usercode request auto-closing the HMD or driver when the onscreen window is closed?
  if hmd{handle}.autoclose > 0
    % Attach a window close callback for Device teardown at window close time:
    if hmd{handle}.autoclose == 2
      % Shutdown driver completely:
      Screen('Hookfunction', win, 'AppendMFunction', 'CloseOnscreenWindowPreGLShutdown', 'Shutdown window callback into PsychOpenXR driver.', 'PsychOpenXR(''Close'');');
    else
      % Only close this HMD:
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
      Screen('Flip', win);
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

  % Tracked operation requested?
  if ~isempty(strfind(hmd{handle}.basicTask, 'Tracked'))
    % 3D head tracked VR rendering task: Start tracking as a convenience:
    PsychOpenXRCore('Start', handle);
  end

  % Last step: Start presenter thread if always-on multi-threading is requested:
  if hmd{handle}.multiThreaded == 2
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
