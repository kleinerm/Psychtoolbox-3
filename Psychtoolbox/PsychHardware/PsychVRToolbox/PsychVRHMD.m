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
% hmd = PsychVRHMD('AutoSetupHMD' [, basicTask][, basicQuality][, vendor][, deviceIndex]);
% - Auto detect the first connected HMD, set it up with reasonable
% default parameters, and return a device handle 'hmd' to it.
%
% Optional parameters: 'basicTask' what kind of task should be implemented.
% The default is 'Tracked3DVR', which means to setup for stereoscopic 3D
% rendering, driven by head motion tracking, for a fully immersive experience
% in some kind of 3D virtual world. This is the default if omitted. The task
% 'Stereoscopic' sets up for display of stereoscopic stimuli, but without
% head tracking. 'Monoscopic' sets up for display of monocular stimuli, ie.
% the HMD is just used as a special kind of standard display monitor.
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

% History:
% 23-Sep-2015  mk  Written.

% Global GL handle for access to OpenGL constants needed in setup:
global GL;

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
    basicQuality = varargin{2};
  else
    basicQuality = 0;
  end

  if length(varargin) >= 4 && ~isempty(varargin{4})
    deviceIndex = varargin{4};
  else
    deviceIndex = [];
  end

  if length(varargin) >= 3 && ~isempty(varargin{3})
    vendor = varargin{3};
    if strcmpi(vendor, 'Oculus')
      hmd = PsychOculusVR('AutoSetupHMD', basicTask, basicQuality, deviceIndex)
    end

    error('AutoSetupHMD: Invalid ''vendor'' requested. This vendor is not supported.');
  end

  % Probe sequence:
  hmd = [];

  % Oculus runtime supported and online? At least one real HMD connected?
  if PsychOculusVR('Supported') && PsychOculusVR('GetCount') > 0
    % Yes. Use that one. This will also inject a proper PsychImaging task
    % for setup of the imaging pipeline:
    hmd = PsychOculusVR('AutoSetupHMD', basicTask, basicQuality, deviceIndex);

    % Return the handle:
    varargout{1} = hmd;
    return;
  end

  % Add probe and autosetup calls for other HMD vendors here...

  % No success with finding any real supported HMD so far. Try to find a driver
  % that at least supports an emulated HMD for very basic testing:
  if PsychOculusVR('Supported')
    hmd = PsychOculusVR('AutoSetupHMD', basicTask, basicQuality, deviceIndex);
    varargout{1} = hmd;
    return;
  end

  % Add probe for other emulated HMD drivers here ...

  % If we reach this point then it is game over:
  error('Could not autosetup any HMDs, real or emulated, for any HMD vendor. Game over!');
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
