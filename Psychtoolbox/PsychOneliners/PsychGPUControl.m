function [rc,rcExt] = PsychGPUControl(cmd, varargin)
% [rc,rcExt] = PsychGPUControl(cmd, arg); -- Control low-level GPU settings.
%
% PsychGPUControl calls into external helper tools to change certain
% low-level operating settings of your systems graphics card (GPU).
%
% Not all operating systems and GPU's support this. The function will
% do nothing on unsupported OS/GPU combos.
%
% Currently OS/X doesn't support this function at all, and on MS-Windows
% and GNU/Linux, only recent ATI GPU's with recent drivers do support it.
%
% All subfunctions return an optional 'rc' return code of zero on success,
% non-zero on error or if the feature is unsupported.
% 'rcExt' is optional as well and its meaning depends on the subfunction.
%
% Subfunctions and their syntax & meaning:
%
% [rc,rcExt] = PsychGPUControl('SetDitheringEnabled', enableFlag);
% - Depending on the setting of 'enableFlag', either enable (=1) or
% disable (=0) display color dithering on all connected displays.
% 'rcExt' returns the version of the low-level dithering API (currently 1 or 2) 
% that had to be used in order to overcome compatibility issues (Windows/AMD 
% only). 'rcExt' is only non-empty when the API was reported by the low-level
% function.
%
% Under normal circumstances, the GPU should decide itself if dithering
% should be used or not. This function allows you to override the GPU's
% automatic choice.
%
% rc = PsychGPUControl('SetGPUPerformance', gpuPerformance);
% - Select the performance state of the GPU. 'gpuPerformance' can be set to
% 0 if the GPU shall automatically adjust its performance and power-
% consumption, or to one of 10 fixed levels between 1 and 10, where 1 means
% the lowest performance - and power consumption, whereas 10 means the
% highest performance - and maximum power consumption.
%
% If in doubt, choose 10 for best accuracy of visual stimulus onset timing,
% 0 for non-critical activities to leave the decision up to the graphics
% driver and GPU.
%
%
% rc = PsychGPUControl('FullScreenWindowDisablesCompositor', flag [, screenIds]);
% - Select if desktop composition should be disabled for displays where
% a Psychtoolbox fullscreen onscreen window is displayed. 'flag' == 1 means
% to disable composition for fullscreen windows, 0 means to enable composition
% for fullscreen windows. You usually want composition to be disabled, as this
% is currently the only way to get decent timing and precise visual stimulus
% onset timestamping. The optional vector of 'screenIds' selects which screens
% should be affected by the change. If left out or set to [], all detected
% screens will be changed. This will also apply a workaround for a limitation in
% Ubuntu Linux standard Unity GUI (actually in the Compiz compositor) when used on
% a single-x-screen setup with multiple video outputs, e.g., a single x-screen,
% dual-display stereo setup. For the workaround to work, you may have to logout
% and login again once, as sometimes the system does not seem to pick up the new
% settings until a logout/login cycle.
%
%
% rc = PsychGPUControl('EnableCompizMultiDisplayWorkaround', enableFlag);
% - Enable or disable workaround for a limitation of the Ubuntu Linux Unity GUI
% if one wants to do multi-display visual stimulation on a single x-screen setup
% or on X-Screen 0 of a multi x-screen setup. Trying to open a Psychtoolbox
% fullscreen window on PTB screen 0 aka X-Screen 0 if that X-Screen has multiple
% active displays attached, e.g., for dual-display stereo stimulation, can fail
% to work without severe visual stimulation timing problems. This is due to a
% design limitation of Unity's Compiz desktop compositor, as of Ubuntu 16.04.0 LTS
% and earlier distribution versions. You can either solve this problem by switching
% to a different desktop GUI environment like GNOME-3 or KDE, or by attaching all
% visual stimulation displays to a secondary X-Screen, e.g., screen 1 (run XOrgConfCreator
% to set up such a setup). If you want to use Unity, you can use this function to
% enable a workaround that fixes the problem, either immediately or after a logout
% and login. There are no known downsides of the workaround when using your system
% with a single display attached. The downside of the workaround on multi-display
% setups will be that while Psychtoolbox will work fine, during your regular use of
% the desktop GUI, automatic window placement and resizing of regular GUI applications
% may be suboptimal due to the workaround interfering. Windows will no longer maximize
% on a single display screen if you ask them to, but over the whole desktop. You can
% use this function to enable and disable the workaround if you tend to use your desktop
% not only for multi-display stimulation, but also for regular multi-display desktop use.
%
% Set 'enableFlag' to 1 to enable the workaround for visual stimulation, and 0
% to disable the workaround for more ergonomic multi-display desktop use.
%

% History:
%  3.01.2010  mk  Written.
% 19.04.2010  mk  Add quotes around path to command to protect against
%                 blanks in path to executable.
% 16.01.2011  mk  Add function to control desktop composition on Linux with
%                 Compiz.
% 18.04.2013  mk  Add use of 64-Bit ATIRadeonperf_Linux64 exe on 64-Bit Linux.
% 26.04.2016  mk  Add 'EnableCompizMultiDisplayWorkaround' for multi-display setups.
% 10.07.2017  MR  Add 'rcExt' for returning the dithering API version used by ATIRadeonperf_Windows.
%                 Suppress the output of ATIRadeonperf_Windows as long as there are no errors.


if nargin < 1
  error('Subfunction command argument missing!');
end

rcExt = [];
if strcmpi(cmd, 'SetDitheringEnabled')
  if isempty(varargin)
    error('SetDitheringEnabled: enableFlag argument missing!');
  end

  enable = varargin{1};
  if ~ismember(enable, 0:1)
    error('SetDitheringEnabled: Invalid enableFlag argument, not 0 or 1!');
  end

  % Command code 1 means: Control ditherstate, according to 2nd arg 0 or 1 == disable, enable.
  cmdpostfix = sprintf(' 1 %i', enable);
  [rc, rcExt] = executeRadeoncmd(cmdpostfix);
  return;
end

if strcmpi(cmd, 'SetGPUPerformance')
  if isempty(varargin)
    error('SetGPUPerformance: gpuPerformance argument missing!');
  end

  gpuperf = varargin{1};
  if ~ismember(gpuperf, 0:10)
    error('SetGPUPerformance: Invalid gpuPerformance argument, not an integer in range 0 - 10!');
  end

  % Map range 1 to 5 to "minimum performance" on ATI GPU's:
  if gpuperf > 0 && gpuperf <= 5
    perfflag = 2;
  end

  % Map range 6 to 10 to "maximum performance" on ATI GPU's:
  if gpuperf > 5 && gpuperf <= 10
    perfflag = 1;
  end

  if gpuperf == 0
    perfflag = 0;
  end

  % Command code 0 means: Control performance state: 0 = AUTO, 1 = MAX, 2 = MIN.
  cmdpostfix = sprintf(' 0 %i', perfflag);
  rc = executeRadeoncmd(cmdpostfix);
  return;
end

if strcmpi(cmd, 'FullScreenWindowDisablesCompositor')
  if isempty(varargin)
    error('FullScreenWindowDisablesCompositor: flag argument missing!');
  end

  compositorOff = varargin{1};
  if ~ismember(compositorOff, 0:1)
    error('FullScreenWindowsDisableCompositor: Invalid flag argument, not an integer of value 0 or 1!');
  end

  if length(varargin) < 2 || isempty(varargin{2})
    screenIds = Screen('Screens');
  else
    screenIds = varargin{2};
  end

  % Which OS?
  if ~IsLinux
    % Nothing to do on non-Linux as compositor handling is
    % implemented in Screen internally:
    rc = 1;
    return;
  end

  % We only know how to do this for Compiz, so we try that. Settings are persistent
  % across sessions and take effect immediately:
  rc = [];
  for screenId=screenIds
    if compositorOff
      % Enable un-redirection: Fullscreen windows aren't subject to treatment by compositor,
      % but can do (e.g. page-flipping) whatever they want:
      newstate = 'dis';
      rc(end+1) = system(sprintf('gconftool-2 -s --type bool /apps/compiz/general/screen%i/options/unredirect_fullscreen_windows true', screenId));
      rc(end+1) = system(sprintf('gconftool-2 -s --type bool /apps/compiz-1/plugins/composite/screen%i/options/unredirect_fullscreen_windows true', screenId));
      rc(end+1) = system(sprintf('dconf write /org/compiz/profiles/unity/plugins/composite/unredirect-fullscreen-windows true'));
    else
      % Disable un-redirection: Fullscreen windows get composited as all other windows:
      newstate = 'en';
      rc(end+1) = system(sprintf('gconftool-2 -s --type bool /apps/compiz/general/screen%i/options/unredirect_fullscreen_windows false', screenId));
      rc(end+1) = system(sprintf('gconftool-2 -s --type bool /apps/compiz-1/plugins/composite/screen%i/options/unredirect_fullscreen_windows false', screenId));
      rc(end+1) = system(sprintf('dconf write /org/compiz/profiles/unity/plugins/composite/unredirect-fullscreen-windows false'));
    end

    if ~all(rc)
      rc = 0;
      fprintf('PsychGPUControl:FullScreenWindowDisablesCompositor: Desktop composition for fullscreen windows on screen %i %sabled.\n', screenId, newstate);
    else
      rc = 1;
      fprintf('PsychGPUControl:FullScreenWindowDisablesCompositor: FAILED to %sable desktop composition for fullscreen windows on screen %i!\n', newstate, screenId);
      fprintf('This can cause visual onset timing problems! See ''help SyncTrouble'' - the Linux specific subsection for tips.\n');
    end
  end
  return;
end

if strcmpi(cmd, 'EnableCompizMultiDisplayWorkaround')
  if isempty(varargin)
    error('EnableCompizMultiDisplayWorkaround: flag argument missing!');
  end

  workaround = varargin{1};
  if ~ismember(workaround, 0:1)
    error('EnableCompizMultiDisplayWorkaround: Invalid flag argument, not an integer of value 0 or 1!');
  end

  % Which OS?
  if ~IsLinux
    % Nothing to do on non-Linux as this is a Compiz on Linux only thing:
    rc = 1;
    return;
  end

  % Settings are persistent across sessions and (should) take effect immediately:
  rc = [];
  if workaround
    newstate = 'en';
    rc(end+1) = system(sprintf('dconf write /org/compiz/profiles/unity/plugins/core/outputs "[''50000x50000'']"'));
    rc(end+1) = system(sprintf('dconf write /org/compiz/profiles/unity/plugins/core/detect-outputs false'));
  else
    newstate = 'dis';
    rc(end+1) = system(sprintf('dconf write /org/compiz/profiles/unity/plugins/core/detect-outputs true'));
  end

  if ~all(rc)
    rc = 0;
    fprintf('PsychGPUControl:EnableCompizMultiDisplayWorkaround: Compiz workaround %sabled.\n', newstate);
    fprintf('On some setups it may be neccessary to logout and login again for the workaround to be effective.\n');
  else
    rc = 1;
    fprintf('PsychGPUControl:EnableCompizMultiDisplayWorkaround: FAILED to %sable Compiz workaround!\n', newstate);
    fprintf('This can cause visual onset timing problems on setups with multiple stimulation displays attached to screen 0!\n');
    fprintf('However, single-display setups, or multi-display setups on separate X-Screens, should be unaffected. Other desktop\n');
    fprintf('environments like GNOME-3 or KDE, would not be affected either.\n');
  end

  return;
end

error('Invalid subfunction provided. Read the help for valid commands!');
return; %#ok<UNRCH>
end

function [rc, rcExt] = executeRadeoncmd(cmdpostfix)

    rcExt = [];
    if IsOSX
        % A no-op on OS/X, as this is not supported at all.
        rc = 1;
        return;
    end

    if IsLinux
        % For 32-Bit Linux:
        cmdprefix = '/PsychContributed/ATIRadeonperf_Linux';
    end

    if IsLinux(1)
        % For 64-Bit Linux: We have a dedicated 64-Bit executable, so we
        % do not require installation of 32-Bit compatibility libraries on
        % 64-Bit Linux systems just for our little exe here, as that would
        % be wasteful:
        cmdprefix = '/PsychContributed/ATIRadeonperf_Linux64';
    end

    if IsWin
        % For 32-Bit or 64-Bit Windows we always use a 32-Bit executable:
        cmdprefix = '/PsychContributed/ATIRadeonperf_Windows.exe';
    end

    % Does the executable exist? Otherwise error out early:
    if ~exist([PsychtoolboxRoot cmdprefix], 'file')
        rc = 1;
        return;
    end

    % Create quoted version of path, so blanks in path are handled properly:
    doCmd = strcat('"', [PsychtoolboxRoot cmdprefix ' '] ,'"');

    % Call final command, return its return status code:
    [rc, msg] = system([doCmd cmdpostfix]);

    % Code has it backwards 1 = success, 0 = failure. Remap to our
    % convention:
    rc = 1 - rc;

    if IsWin
        % Extract dithering API version.
        i = strfind(msg, 'dithering API v');
        if ~isempty(i)
            rcExt = sscanf(msg(i(1):end),'dithering API v%d');
        end
    end
    
    % Output potential status or error messages, unless it is a message
    % that signals we are not executing on a AMD/ATI GPU with Catalyst
    % driver, ie., that the whole thing was a no-op:
    if rc~=0 && isempty(strfind(msg, 'ADL library not found!'))
        disp(msg);
    end

    return;
end
