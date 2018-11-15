function varargout = PsychOculusVR1(cmd, varargin)
% PsychOculusVR1 - A high level driver for Oculus VR hardware using the Version 1.11+ runtime.
%
% Note: If you want to write VR code that is portable across
% VR headsets of different vendors, then use the PsychVRHMD()
% driver instead of this driver. The PsychVRHMD driver will use
% this driver as appropriate when connecting to a Oculus Rift
% or similar Oculus device, but it will also automatically work
% with other head mounted displays. This driver does however
% expose a few functions specific to Oculus hardware, so you can
% mix calls to this driver with calls to PsychVRHMD to do some
% mix & match.
%
% For setup instructions for Oculus HMDs see "help OculusVR".
%
%
% Usage:
%
% hmd = PsychOculusVR1('AutoSetupHMD' [, basicTask='Tracked3DVR'][, basicRequirements][, basicQuality=0][, deviceIndex]);
% - Open a Oculus HMD, set it up with good default rendering and
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
% the HMD is just used as a special kind of standard display monitor.
%
% 'basicRequirements' defines basic requirements for the task. Currently
% defined are the following strings which can be combined into a single
% 'basicRequirements' string:
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
% hmd = PsychOculusVR1('Open' [, deviceIndex], ...);
% - Open HMD with index 'deviceIndex'. See PsychOculusVRCore1 Open?
% for help on additional parameters.
%
%
% PsychOculusVR1('SetAutoClose', hmd, mode);
% - Set autoclose mode for HMD with handle 'hmd'. 'mode' can be
% 0 (this is the default) to not do anything special. 1 will close
% the HMD 'hmd' when the onscreen window is closed which displays
% on the HMD. 2 will do the same as 1, but close all open HMDs and
% shutdown the complete driver and Oculus runtime - a full cleanup.
%
%
% isOpen = PsychOculusVR1('IsOpen', hmd);
% - Returns 1 if 'hmd' corresponds to an open HMD, 0 otherwise.
%
%
% PsychOculusVR1('Close' [, hmd])
% - Close provided HMD device 'hmd'. If no 'hmd' handle is provided,
% all HMDs will be closed and the driver will be shutdown.
%
%
% info = PsychOculusVR1('GetInfo', hmd);
% - Retrieve a struct 'info' with information about the HMD 'hmd'.
% The returned info struct contains at least the following standardized
% fields with information:
%
% handle = Driver internal handle for the specific HMD.
% driver = Function handle to the actual driver for the HMD, e.g., @PsychOculusVR1.
% type   = Defines the type/vendor of the device, e.g., 'Oculus'.
% modelName = Name string with the name of the model of the device, e.g., 'Rift DK2'.
% separateEyePosesSupported = 1 if use of PsychOculusVR1('GetEyePose') will improve
%                             the quality of the VR experience, 0 if no improvement
%                             is to be expected, so 'GetEyePose' can be avoided
%                             to save processing time without a loss of quality.
%                             This always returns 1 for at least the Rift DK1 and DK2,
%                             as use of that function can enhance the quality of the
%                             VR experience with fast head movements.
%
% The returned struct may contain more information, but the fields mentioned
% above are the only ones guaranteed to be available over the long run. Other
% fields may disappear or change their format and meaning anytime without
% warning.
%
%
% isSupported = PsychOculusVR1('Supported');
% - Returns 1 if the Oculus driver is functional, 0 otherwise. The
% driver is functional if the VR runtime library was successfully
% initialized and a connection to the VR server process has been
% established. It would return 0 if the server process would not be
% running, or if the required runtime library would not be correctly
% installed.
%
%
% state = PsychOculusVR1('PrepareRender', hmd [, userTransformMatrix][, reqmask=1][, targetTime]);
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
% position tracker like, e.g., the Oculus Rift DK2 camera.
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
% More flags to follow...
%
%
% eyePose = PsychOculusVR1('GetEyePose', hmd, renderPass [, userTransformMatrix][, targetTime]);
% - Return a struct 'eyePose' which contains various useful bits of information
% for 3D stereoscopic rendering of the stereo view of one eye, based on head
% tracking data. This function provides essentially the same information as
% the 'PrepareRender' function, but only for one eye. Therefore you will need
% to call this function twice, once for each of the two renderpasses, at the
% beginning of each renderpass.
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
% Additionally tracked/predicted head pose is returned in eyePose.localHeadPoseMatrix
% and the global head pose after application of the 'userTransformMatrix' is
% returned in eyePose.globalHeadPoseMatrix - this is the basis for computing
% the camera transformation matrix.
%
%
% PsychOculusVR1('SetupRenderingParameters', hmd [, basicTask='Tracked3DVR'][, basicRequirements][, basicQuality=0][, fov=[HMDRecommended]][, pixelsPerDisplay=1])
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
% user.
%
% 'pixelsPerDisplay' Ratio of the number of render target pixels to display pixels
% at the center of distortion. Defaults to 1.0 if omitted. Lower values can
% improve performance, at lower quality.
%
%
% PsychOculusVR1('SetBasicQuality', hmd, basicQuality);
% - Set basic level of quality vs. required GPU performance.
%
%
% oldSetting = PsychOculusVR1('SetFastResponse', hmd [, enable]);
% - Return old setting for 'FastResponse' mode in 'oldSetting',
% optionally disable or enable the mode via specifying the 'enable'
% parameter as 0 or greater than zero.
%
% Deprecated: This function does nothing. It just exists for (backwards)
% compatibility with the old Oculus driver and PsychVRHMD.
%
%
% oldSetting = PsychOculusVR1('SetTimeWarp', hmd [, enable]);
% - Return old setting for 'TimeWarp' mode in 'oldSetting',
% optionally enable or disable the mode via specifying the 'enable'
% parameter as 1 or 0.
%
% Deprecated: This function does nothing. It just exists for (backwards)
% compatibility with the old Oculus driver and PsychVRHMD.
%
%
% oldSetting = PsychOculusVR1('SetLowPersistence', hmd [, enable]);
% - Return old setting for 'LowPersistence' mode in 'oldSetting',
% optionally enable or disable the mode via specifying the 'enable'
% parameter as 1 or 0.
%
% Deprecated: This function does nothing. It just exists for (backwards)
% compatibility with the old Oculus driver and PsychVRHMD.
%
%
% oldSettings = PsychOculusVR1('PanelOverdriveParameters', hmd [, newparams]);
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
% PsychOculusVR1('SetHSWDisplayDismiss', hmd [, dismissTypes=1+2+4]);
% - Set how the user can dismiss the "Health and safety warning display".
% Deprecated: This function does nothing. It just exists for (backwards)
% compatibility with the old Oculus driver and PsychVRHMD. The current OculusVR
% runtime 1.0 enforces display of the Health and Safety Warning at the beginning
% of each VR session.
%
% [bufferSize, imagingFlags, stereoMode] = PsychOculusVR1('GetClientRenderingParameters', hmd);
% - Retrieve recommended size in pixels 'bufferSize' = [width, height] of the client
% renderbuffer for each eye for rendering to the HMD. Returns parameters
% previously computed by PsychOculusVR1('SetupRenderingParameters', hmd).
%
% Also returns 'imagingFlags', the required imaging mode flags for setup of
% the Screen imaging pipeline. Also returns the needed 'stereoMode' for the
% pipeline.
%
%
% isOutput = PsychOculusVR1('IsHMDOutput', hmd, scanout);
% - Returns 1 (true) if 'scanout' describes the video output to which the
% HMD 'hmd' is connected. 'scanout' is a struct returned by the Screen
% function Screen('ConfigureDisplay', 'Scanout', screenid, outputid);
% This allows probing video outputs to find the one which feeds the HMD.
% Deprecated: This function does nothing. It just exists for (backwards)
% compatibility with the old Oculus driver and PsychVRHMD.
%
%
% [headToEyeShiftv, headToEyeShiftMatrix] = PsychOculusVR1('GetEyeShiftVector', hmd, eye);
% - Retrieve 3D translation vector [tx, ty, tz] that defines the 3D position of the given
% eye 'eye' for the given HMD 'hmd', relative to the origin of the local head/HMD
% reference frame. This is needed to translate a global head pose into a eye
% pose, e.g., to translate the output of PsychOculusVR1('GetEyePose') into actual
% tracked/predicted eye locations for stereo rendering.
%
% In addition to the 'headToEyeShiftv' vector, a corresponding 4x4 translation
% matrix is also returned in 'headToEyeShiftMatrix' for convenience.
%
%

% Global GL handle for access to OpenGL constants needed in setup:
global GL;

persistent hmd;

if nargin < 1 || isempty(cmd)
  help PsychOculusVR1;
  fprintf('\n\nAlso available are functions from PsychOculusVRCore1:\n');
  PsychOculusVRCore1;
  return;
end

% Fast-Path function 'Present' - Presents new frames to Compositor:
if cmd == 1
  handle = varargin{1};

  % Execute VR Present: Submit just unbound textures, start render/warp/presentation
  % on HMD at next possible point in time:
  PsychOculusVRCore1('EndFrameTiming', hmd{handle}.handle);

  % Get next set of backing textures for next Screen() post-flip drawing/render cycle
  % from the OculusVR texture swap chains:
  texLeft = PsychOculusVRCore1('GetNextTextureHandle', hmd{handle}.handle, 0);
  if hmd{handle}.StereoMode > 0
      texRight = PsychOculusVRCore1('GetNextTextureHandle', hmd{handle}.handle, 1);
  else
      texRight = [];
  end

  % Attach them as new backing textures, detach the previously bound ones, so they
  % are ready for submission to the VR compositor:
  [oldL, oldR] = Screen('Hookfunction', hmd{handle}.win, 'SetDisplayBufferTextures', '', texLeft, texRight);

  % Disable presentation in onscreen window and associated throttling and standard Flip timestamping:
  Screen('Hookfunction', hmd{handle}.win, 'SetOneshotFlipFlags', '', kPsychSkipSwapForFlipOnce + kPsychSkipTimestampingForFlipOnce);
  Screen('Hookfunction', hmd{handle}.win, 'SetOneshotFlipResults', '', GetSecs);

  return;
end

% Fast-Path function 'Cleanup' - Cleans up before onscreen window close/GL shutdown:
if cmd == 2
  handle = varargin{1};

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
    error('PsychOculusVR1:PrepareRender: Specified handle does not correspond to an open HMD!');
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

  % Mark start of a new frame render cycle for the runtime and get the data
  % predicted for next scanout time:
  [eyePose{1}, eyePose{2}, tracked] = PsychOculusVRCore1('StartRender', myhmd.handle);

  % Always return basic tracking status:
  result.tracked = tracked;

  % As a bonus we return the raw eye pose vectors, given that we have them anyway:
  result.rawEyePose7{1} = eyePose{1};
  result.rawEyePose7{2} = eyePose{2};

  % Want matrices which take a usercode supplied global transformation into account?
  if bitand(reqmask, 1)
    % Yes: We need tracked + predicted head pose, so we can apply the user transform,
    % and then per-eye transforms:

    % Get predicted head pose for targetTime:
    state = PsychOculusVRCore1('GetTrackingState', myhmd.handle, targetTime);

    % Bonus feature: HeadPose as 7 component translation + orientation quaternion vector:
    result.headPose = state.HeadPose;

    % Convert head pose vector to 4x4 OpenGL right handed reference frame matrix:
    result.localHeadPoseMatrix = eyePoseToCameraMatrix(state.HeadPose);

    % Premultiply usercode provided global transformation matrix:
    result.globalHeadPoseMatrix = userTransformMatrix * result.localHeadPoseMatrix;

    % Compute per-eye global pose matrices:
    result.cameraView{1} = result.globalHeadPoseMatrix * hmd{myhmd.handle}.eyeShiftMatrix{1};
    result.cameraView{2} = result.globalHeadPoseMatrix * hmd{myhmd.handle}.eyeShiftMatrix{2};

    % Compute inverse matrices, useable as OpenGL GL_MODELVIEW matrices for rendering:
    result.modelView{1} = inv(result.cameraView{1});
    result.modelView{2} = inv(result.cameraView{2});
  end

  varargout{1} = result;

  return;
end

if strcmpi(cmd, 'GetEyePose')
  % Get and validate handle - fast path open coded:
  myhmd = varargin{1};
  if ~((length(hmd) >= myhmd.handle) && (myhmd.handle > 0) && hmd{myhmd.handle}.open)
    error('PsychOculusVR1:GetEyePose: Specified handle does not correspond to an open HMD!');
  end

  % Valid: Get view render pass for which to return information:
  if length(varargin) < 2 || isempty(varargin{2})
    error('PsychOculusVR1:GetEyePose: Required ''renderPass'' argument missing.');
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
  % NOTE: Currently not used, as Oculus SDK v1 does not support passing
  % targetTime into the underlying SDK function for 'GetEyePose'. The
  % Oculus runtime predicts something meaningful internally.
  %
  %  if length(varargin) >= 4 && ~isempty(varargin{4})
  %    targetTime = varargin{4};
  %  else
  %    % Default: Provide predicted value for the midpoint of the next video
  %    % refresh cycle - assuming we hit the flip deadline for the next video
  %    % frame, so that point in time will be exactly in the middle of both
  %    % eyes:
  %    targetTime = [];
  %  end

  % Get eye pose for this renderPass, or more exactly the headPose from which this
  % renderPass eyePose will get computed:
  [result.eyePose, result.eyeIndex] = PsychOculusVRCore1('GetEyePose', myhmd.handle, renderPass);

  % Convert head pose vector to 4x4 OpenGL right handed reference frame matrix:
  result.localEyePoseMatrix = eyePoseToCameraMatrix(result.eyePose);

  % Premultiply usercode provided global transformation matrix:
  result.globalEyePoseMatrix = userTransformMatrix * result.localEyePoseMatrix;

  % Compute per-eye global pose matrix for this eyeIndex:
  result.cameraView = result.globalEyePoseMatrix;

  % Compute inverse matrix, useable as OpenGL GL_MODELVIEW matrix for rendering:
  result.modelView = inv(result.cameraView);

  varargout{1} = result;

  return;
end

if strcmpi(cmd, 'Supported')
  % Check if the Oculus VR runtime 1.x is supported and active on this
  % installation, so it can be used to open connections to real HMDs,
  % or at least to emulate a HMD for simple debugging purposes:
  try
    if exist('PsychOculusVRCore1', 'file') && PsychOculusVRCore1('GetCount') >= 0
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
  if ~PsychOculusVR1('Supported')
    % Nope: Game over.
    fprintf('PsychOculusVR1:AutoSetupHMD: Could not initialize Oculus V1 driver. Game over!\n');

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
    newhmd = PsychOculusVR1('Open', deviceIndex);
  else
    % Check if at least one Oculus HMD is connected and available:
    if PsychOculusVR1('GetCount') > 0
      % Yes. Open and initialize connection to first detected HMD:
      fprintf('PsychOculusVR1: Opening the first connected Oculus VR headset.\n');
      newhmd = PsychOculusVR1('Open', 0);
    else
      % TODO: HMD emulation under 1.x runtime possible?
      % No. Open an emulated/simulated HMD for basic testing and debugging:
      %fprintf('PsychOculusVR1: No Oculus HMD detected. Opening a simulated HMD.\n');
      %newhmd = PsychOculusVR1('Open', -1);
      fprintf('PsychOculusVR1: No Oculus HMD detected. Game over.\n');
      varargout{1} = [];
      return;
    end
  end

  % Trigger an automatic device close at onscreen window close for the HMD display window:
  PsychOculusVR1('SetAutoClose', newhmd, 1);

  % Setup default rendering parameters:
  PsychOculusVR1('SetupRenderingParameters', newhmd, basicTask, basicRequirements, basicQuality);

  % Add a PsychImaging task to use this HMD with the next opened onscreen window:
  PsychImaging('AddTask', 'General', 'UseVRHMD', newhmd);

  % Return the device handle:
  varargout{1} = newhmd;

  % Ready.
  return;
end

if strcmpi(cmd, 'SetAutoClose')
  myhmd = varargin{1};

  if ~PsychOculusVR1('IsOpen', myhmd)
    error('PsychOculusVR1:SetAutoClose: Specified handle does not correspond to an open HMD!');
  end

  % Assign autoclose flag:
  hmd{myhmd.handle}.autoclose = varargin{2};

  return;
end

if strcmpi(cmd, 'SetHSWDisplayDismiss')
  myhmd = varargin{1};

  if ~PsychOculusVR1('IsOpen', myhmd)
    error('PsychOculusVR1:SetHSWDisplay: Specified handle does not correspond to an open HMD!');
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
  [handle, modelName, panelXRes, panelYRes, panelHz] = PsychOculusVRCore1('Open', varargin{:});

  newhmd.handle = handle;
  newhmd.driver = @PsychOculusVR1;
  newhmd.type   = 'Oculus';
  newhmd.open = 1;
  newhmd.modelName = modelName;
  newhmd.separateEyePosesSupported = 1; % TODO FIXME Makes still sense?
  newhmd.panelXRes = panelXRes;
  newhmd.panelYRes = panelYRes;
  newhmd.videoRefreshDuration = 1 / panelHz;

  % Default autoclose flag to "no autoclose":
  newhmd.autoclose = 0;

  % By default allow user to dismiss HSW display via key press,
  % mouse click, or HMD tap:
  newhmd.hswdismiss = 1 + 2 + 4;

  % Setup basic task/requirement/quality specs to "nothing":
  newhmd.basicQuality = 0;
  newhmd.basicTask = '';
  newhmd.basicRequirements = '';

  % Store in internal array:
  hmd{handle} = newhmd;

  % Return device struct:
  varargout{1} = newhmd;
  varargout{2} = modelName;

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
  if ~PsychOculusVR1('IsOpen', varargin{1})
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
      PsychOculusVRCore1('Close', myhmd.handle);
      hmd{myhmd.handle}.open = 0;
    end
  else
    % Shutdown whole driver:
    PsychOculusVRCore1('Close');
    hmd = [];
  end

  return;
end

if strcmpi(cmd, 'IsHMDOutput')
  myhmd = varargin{1}; %#ok<NASGU>
  scanout = varargin{2};

  % This does not make much sense on the OculusVR 1.0 version, as that runtime
  % only supports direct output mode, ie. an output completely separate from the
  % regular desktop and windowing system display space. But lets leave the legacy
  % code for the DK2 in place.

  % Is this a Rift DK2 panel?
  if (scanout.width == 1080) && (scanout.height == 1920)
    varargout{1} = 1;
  else
    varargout{1} = 0;
  end
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
    %    PsychOculusVRCore1('SetDynamicPrediction', handle, 1);
  else
    %    PsychOculusVRCore1('SetDynamicPrediction', handle, 0);
  end

  return;
end

if strcmpi(cmd, 'SetFastResponse')
  myhmd = varargin{1};
  if ~PsychOculusVR1('IsOpen', myhmd)
    error('SetFastResponse: Passed in handle does not refer to a valid and open HMD.');
  end
  handle = myhmd.handle;

  % FastResponse has no meaningful implementation on the OculusVR 1.0 runtime,
  % so just return a constant old value of 1 for "fast response always enabled":
  varargout{1} = 1;

  return;
end

if strcmpi(cmd, 'SetTimeWarp')
  myhmd = varargin{1};
  if ~PsychOculusVR1('IsOpen', myhmd)
    error('SetTimeWarp: Passed in handle does not refer to a valid and open HMD.');
  end

  % SetTimeWarp determined use of GPU accelerated 2D texture sampling
  % warp on the Rift DK1/DK2 with old v0.5 SDK. On the 1.0 SDK we no
  % longer have any programmatic control over timewarping,so leave this
  % in place as dummy.

  % Return constant old setting of "TimeWarp always on":
  varargout{1} = 1;

  return;
end

if strcmpi(cmd, 'SetLowPersistence')
  myhmd = varargin{1};
  if ~PsychOculusVR1('IsOpen', myhmd)
    error('SetLowPersistence: Passed in handle does not refer to a valid and open HMD.');
  end

  % SetLowPersistence determined use of low persistence mode on the Rift DK2 with
  % the v0.5 SDK. We don't have control over this on the v1.0 runtime anymore.
  % Return constant old setting "Always low persistence":
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

  PsychOculusVR1('SetBasicQuality', myhmd, basicQuality);

  % Get optimal client renderbuffer size - the size of our virtual framebuffer for left eye:
  [hmd{myhmd.handle}.rbwidth, hmd{myhmd.handle}.rbheight, hmd{myhmd.handle}.fov] = PsychOculusVRCore1('GetFovTextureSize', myhmd.handle, 0, varargin{5:end});

  % Get optimal client renderbuffer size - the size of our virtual framebuffer for right eye:
  [hmd{myhmd.handle}.rbwidth, hmd{myhmd.handle}.rbheight, hmd{myhmd.handle}.fov] = PsychOculusVRCore1('GetFovTextureSize', myhmd.handle, 1, varargin{5:end});

  return;
end

if strcmpi(cmd, 'GetClientRenderingParameters')
  myhmd = varargin{1};
  varargout{1} = [hmd{myhmd.handle}.rbwidth, hmd{myhmd.handle}.rbheight];

  % We need fast backing store support for imaging pipeline with virtual framebuffers,
  % also output redirection for providing final output to us, instead of displaying
  % into the standard onscreen window. The OculusVR 1.x runtime generates its own
  % textures to be used as externally injected color buffer backing textures:
  imagingMode = mor(kPsychNeedFastBackingStore, kPsychNeedFinalizedFBOSinks, kPsychUseExternalSinkTextures);

  % TODO FIXME: Support MSAA multisample resolve in the VR compositor instead of
  % PTB imaging pipeline for higher efficiency? Possible with current 1.11 runtime?
  % imagingMode = mor(imagingMode, kPsychSinkIsMSAACapable);

  if ~strcmpi(hmd{myhmd.handle}.basicTask, 'Monoscopic')
    % We must use stereomode 12, so we get separate draw buffers for left and
    % right eye, and separate stream processing into our VR runtime provided
    % separate backing textures / texture swap chains per eye, with all internal
    % buffers at at least full VR compositor input resolution.
    stereoMode = 12;
  else
    % Monoscopic presentation will do:
    stereoMode = 0;
  end

  varargout{2} = imagingMode;
  varargout{3} = stereoMode;
  return;
end

if strcmpi(cmd, 'GetEyeShiftVector')
  myhmd = varargin{1};

  if varargin{2} == 0
    varargout{1} = hmd{myhmd.handle}.HmdToEyeViewOffsetLeft;
    varargout{2} = hmd{myhmd.handle}.eyeShiftMatrix{1};
  else
    varargout{1} = hmd{myhmd.handle}.HmdToEyeViewOffsetRight;
    varargout{2} = hmd{myhmd.handle}.eyeShiftMatrix{2};
  end

  return;
end

if strcmpi(cmd, 'PerformPostWindowOpenSetup')
  % Must have global GL constants:
  if isempty(GL)
    varargout{1} = 0;
    warning('PTB internal error in PsychOculusVR1: GL struct not initialized?!?');
    return;
  end

  % Oculus device handle:
  myhmd = varargin{1};
  handle = myhmd.handle;

  % Onscreen window handle:
  win = varargin{2};
  winfo = Screen('GetWindowInfo', win);
  hmd{handle}.StereoMode = winfo.StereoMode;

  % Keep track of window handle of associated onscreen window:
  hmd{handle}.win = win;

  % Need to know user selected clearcolor:
  clearcolor = varargin{3};

  % Compute effective size of per-eye input buffer for undistortion render.
  % The input buffers for undistortion are the finalizedDrawbufferFBO's, ie.
  % the final output buffers of Psychtoolbox imaging pipeline.
  % This means [inputWidth, inputHeight] == [rbwidth, rbheight].
  hmd{handle}.inputWidth = hmd{handle}.rbwidth;
  hmd{handle}.inputHeight = hmd{handle}.rbheight;

  % Query undistortion parameters for left eye view:
  [hmd{handle}.rbwidth, hmd{handle}.rbheight, vx, vy, vw, vh, ptx, pty, hsx, hsy, hsz] = PsychOculusVRCore1('GetUndistortionParameters', handle, 0, hmd{handle}.inputWidth, hmd{handle}.inputHeight, hmd{handle}.fov);
  hmd{handle}.viewportLeft = [vx, vy, vw, vh];
  hmd{handle}.PixelsPerTanAngleAtCenterLeft = [ptx, pty];
  hmd{handle}.HmdToEyeViewOffsetLeft = [hsx, hsy, hsz];

  % Query parameters for right eye view:
  [hmd{handle}.rbwidth, hmd{handle}.rbheight, vx, vy, vw, vh, ptx, pty, hsx, hsy, hsz] = PsychOculusVRCore1('GetUndistortionParameters', handle, 1, hmd{handle}.inputWidth, hmd{handle}.inputHeight, hmd{handle}.fov);
  hmd{handle}.viewportRight = [vx, vy, vw, vh];
  hmd{handle}.PixelsPerTanAngleAtCenterRight = [ptx, pty];
  hmd{handle}.HmdToEyeViewOffsetRight = [hsx, hsy, hsz];

  % Convert head to eye shift vectors into 4x4 matrices, as we'll need
  % them frequently:
  EyeT = diag([1 1 1 1]);
  EyeT(1:3, 4) = hmd{handle}.HmdToEyeViewOffsetLeft';
  hmd{handle}.eyeShiftMatrix{1} = EyeT;

  EyeT = diag([1 1 1 1]);
  EyeT(1:3, 4) = hmd{handle}.HmdToEyeViewOffsetRight';
  hmd{handle}.eyeShiftMatrix{2} = EyeT;

  % Create texture swap chains to provide textures to be used for
  % frame submission to the VR compositor:

  % Left eye / mono chain:
  [width, height, numTextures] = PsychOculusVRCore1('CreateRenderTextureChain', hmd{handle}.handle, 0, hmd{handle}.inputWidth, hmd{handle}.inputHeight);

  % Create 2nd chain for right eye in stereo mode:
  if winfo.StereoMode > 0
    if winfo.StereoMode ~=12
      sca;
      error('Invalid Screen() StereoMode in use for OculusVR HMD! Must be 12 or 0.');
    end
    [width, height, numTextures] = PsychOculusVRCore1('CreateRenderTextureChain', hmd{handle}.handle, 1, hmd{handle}.inputWidth, hmd{handle}.inputHeight);
  end

  % Query currently bound finalizedFBO backing textures, to keep them around as
  % backups for restoration when closing down the session:
  [hmd{handle}.oldglLeftTex, hmd{handle}.oldglRightTex, textarget, texformat, texmultisample, texwidth, texheight] = Screen('Hookfunction', win, 'GetDisplayBufferTextures');
  if (textarget ~= GL.TEXTURE_2D) || (texformat ~= GL.RGBA8) || (texmultisample ~= 0)
    sca;
    error('Invalid Screen() backing textures required. Non-matching texture target, format or multisample setting.');
  end

  if (texwidth ~= width) || (texheight ~= height)
    sca;
    error('Invalid Screen() backing textures required. Non-matching width x height.');
  end

  if hmd{handle}.oldglRightTex == 0
    hmd{handle}.oldglRightTex = [];
  end

  % Get and clear all backing textures from the VR compositor:
  clearvalues = ones(4, texwidth, texheight, 'single');
  for i=1:numTextures
    % Get backing textures provided by the textures swap chains of the OculusVR compositor
    % and clear them to black color by zero-filling:
    texLeft = PsychOculusVRCore1('GetNextTextureHandle', hmd{handle}.handle, 0);
    glBindTexture(textarget, texLeft);
    glTexSubImage2D(textarget, 0, 0, 0, texwidth, texheight, GL.RGBA, GL.FLOAT, clearvalues);

    if winfo.StereoMode > 0
      texRight = PsychOculusVRCore1('GetNextTextureHandle', hmd{handle}.handle, 1);
      glBindTexture(textarget, texRight);
      glTexSubImage2D(textarget, 0, 0, 0, texwidth, texheight, GL.RGBA, GL.FLOAT, clearvalues);
    else
      texRight = [];
    end
    glBindTexture(textarget, 0);

    % Execute VR Present: Submit just unbound textures, start render/warp/presentation
    % on HMD at next possible point in time:
    PsychOculusVRCore1('EndFrameTiming', hmd{handle}.handle);
  end
  clear clearvalues;

  % Get first textures for actual use in PTB's imaging pipeline:
  texLeft = PsychOculusVRCore1('GetNextTextureHandle', hmd{handle}.handle, 0);
  if hmd{handle}.StereoMode > 0
      texRight = PsychOculusVRCore1('GetNextTextureHandle', hmd{handle}.handle, 1);
  else
      texRight = [];
  end

  % Assign them to Screen():
  Screen('Hookfunction', win, 'SetDisplayBufferTextures', '', texLeft, texRight);

  % Go back to user requested clear color, now that all our buffers
  % are cleared to black:
  Screen('FillRect', win, clearcolor);

  % Need to call the PsychOculusVR1(1) callback at each Screen('Flip') to get the
  % imaging pipeline post-processed final output frames for left/right eye and submit
  % them to the VR-Compositor for presentation on the HMD:
  cmdString = sprintf('PsychOculusVR1(1, %i);', handle);
  Screen('Hookfunction', win, 'AppendMFunction', 'PreSwapbuffersOperations', 'OculusVR Present Operation', cmdString);
  Screen('Hookfunction', win, 'Enable', 'PreSwapbuffersOperations');

  % Attach shutdown procedure on onscreen window close:
  cmdString = sprintf('PsychOculusVR1(2, %i);', handle);
  Screen('Hookfunction', win, 'PrependMFunction', 'CloseOnscreenWindowPreGLShutdown', 'OculusVR cleanup', cmdString);
  Screen('Hookfunction', win, 'Enable', 'CloseOnscreenWindowPreGLShutdown');

  % Does usercode request auto-closing the HMD or driver when the onscreen window is closed?
  if hmd{handle}.autoclose > 0
    % Attach a window close callback for Device teardown at window close time:
    if hmd{handle}.autoclose == 2
      % Shutdown driver completely:
      Screen('Hookfunction', win, 'AppendMFunction', 'CloseOnscreenWindowPostGLShutdown', 'Shutdown window callback into PsychOculusVR1 driver.', 'PsychOculusVR1(''Close'');');
    else
      % Only close this HMD:
      Screen('Hookfunction', win, 'PrependMFunction', 'CloseOnscreenWindowPostGLShutdown', 'Shutdown window callback into PsychOculusVR1 driver.', sprintf('PsychOculusVR1(''Close'', %i);', handle));
    end
    Screen('HookFunction', win, 'Enable', 'CloseOnscreenWindowPostGLShutdown');
  end

  if ~isempty(strfind(hmd{myhmd.handle}.basicTask, 'Tracked3DVR'))
    % 3D head tracked VR rendering task: Start tracking as a convenience:
    PsychOculusVRCore1('Start', handle);
  end

  % Return success result code 1:
  varargout{1} = 1;
  return;
end

% 'cmd' so far not dispatched? Let's assume it is a command
% meant for PsychOculusVRCore1:
if (length(varargin) >= 1) && isstruct(varargin{1})
  myhmd = varargin{1};
  handle = myhmd.handle;
  [ varargout{1:nargout} ] = PsychOculusVRCore1(cmd, handle, varargin{2:end});
else
  [ varargout{1:nargout} ] = PsychOculusVRCore1(cmd, varargin{:});
end

return;

end
