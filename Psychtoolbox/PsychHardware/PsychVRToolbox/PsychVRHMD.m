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
% hmd = PsychVRHMD('AutoSetupHMD' [, vendor])
% - Auto detect the first connected HMD, set it up with reasonable
% default parameters, and return a device handle 'hmd' to it.
%
% By default all currently supported types of HMDs from different
% vendors are probed and the first one found is used. If the optional
% parameter 'vendor' is provided, only devices from that vendor are
% detected and the first detected device is chosen.
%
%
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
    vendor = varargin{1};
    if ~strcmpi(vendor, 'Oculus')
      error('AutoSetupDefaultHMD: Invalid ''vendor'' requested. This vendor is not supported.');
    end
  end

  % Probe sequence:
  hmd = [];

  % Oculus runtime supported and online? At least one real HMD connected?
  if PsychOculusVR('Supported') && PsychOculusVR('GetCount') > 0
    % Yes. Use that one. This will also inject a proper PsychImaging task
    % for setup of the imaging pipeline:
    hmd = PsychOculusVR('AutoSetupDefaultHMD')

    % Return the handle:
    varargout{1} = hmd;
    return;
  end

  % Add probe and autosetup calls for other HMD vendors here...

  % No success with finding any real supported HMD so far. Try to find a driver
  % that at least supports an emulated HMD for very basic testing:
  if PsychOculusVR('Supported')
    hmd = PsychOculusVR('AutoSetupDefaultHMD')
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
