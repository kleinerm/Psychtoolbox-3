function XOrgConfCreator(expertmode)
% XOrgConfCreator([expertmode=0]) - Automatically create X11 config files.
%
% This friendly little setup assistant will analyze your systems graphics
% card and display setup, then ask you questions about how you want your
% display setup configured. Then it will create a configuration file to
% setup your displays in the desired way after the next logout -> login
% cycle, or after the next reboot of your machine.
%
% Files are stored by default in a configuration directory where the
% XOrgConfSelector helper script can find and apply them.
%
% Please note that this assistant can't yet handle systems with multiple
% active graphics cards properly. It is only for regular single graphics
% card setups or hybrid graphics laptops with one active graphics card.
%
% In order for the assistant to detect and handle all potentially useful
% displays you must have all of them connected and active at the time the
% assistant is run.
%
%
% expertmode = Optional parameter: If set to 1, enable additional options which
%              are only useful for debugging by developers and mostly useless or
%              troublesome for normal users. Defaults to 0 == off.

% History:
% 04-Nov-2015  mk  Written.
% 25-Apr-2016  mk  Add support for selection of modesetting ddx on XOrg 1.18+
% 28-Aug-2016  mk  Add basic hybrid graphics setup support.
% 09-Jun-2017  mk  Add 30 bpp framebuffer support and 16 bpc framebuffer support
%                  on AMD Sea Islands gpus, to simplify setup of high color bit
%                  depth framebuffers.
% 11-Jun-2022  mk  Only handle systems with XOrg X-Server 21 or later, bail to
%                  LegacyXOrgConfCreator for older systems.

clc;

if ~IsLinux
  fprintf('This function is only supported or useful on Linux. Bye!\n');
  return;
end

if IsWayland
  fprintf('This function is only supported or useful on Linux with the good old X11/XServer stack, not on Wayland. Bye!\n');
  return;
end

% Expert mode off by default:
if nargin < 1 || isempty(expertmode)
  expertmode = 0;
end

% Which X-Server version is in use?
xversion = [0, 0, 0];
[rc, text] = system('xdpyinfo | grep ''X.Org version''');
if rc == 0
  xversion = sscanf (text, 'X.Org version: %d.%d.%d');
else
  % No xdpyinfo installed by default, e.g., on RaspberryPi OS, or failed?
  % Try again at old traditional xorg log location:
  [rc, text] = system('grep ''X.Org X Server '' /var/log/Xorg.0.log');
  if rc == 0
    xversion = sscanf (text, 'X.Org X Server %d.%d.%d');
  end
end

fprintf('Detected X-Server version %i.%i.%i\n', xversion(1), xversion(2), xversion(3));

% X-Server older than 21.0.0 aka older than 1.21.0?
if (xversion(1) == 1) && (xversion(2) < 21)
    % Yes. Run LegacyXOrgConfCreator() for handling legacy stuff:
    LegacyXOrgConfCreator;
    return;
end

% X-Server 21 or later in use - We are the one to handle the modern XOrg stack.
fprintf('Creating configuration for a modern X-Server 21+, assuming Mesa 22+ and Linux 5.15+ ...\n');

% Step 1: Get the currently active display gpu and derive the required
% X video driver from it. We do this by opening a little invisble
% onscreen window, then querying its window info to get the type
% of primary display gpu used:
fprintf('Detecting type of primary display graphics card (GPU) and driver to use...\n');
oldVerbosity = Screen('Preference', 'Verbosity', 1);
oldSyncTests = Screen('Preference', 'SkipSyncTests', 2);

try
  win = Screen('OpenWindow', 0, 0, [0 0 32 32], 24);
  winfo = Screen('GetWindowInfo', win);
  modesettingddxactive = Screen('GetWindowInfo', win, 8);
  Screen('CloseAll');

  xdriver = DetectDDX(winfo);
  fprintf('Primary display gpu type: %s\n', xdriver);
  if ~modesettingddxactive
    fprintf('Uses the xf86-video-%s DDX video driver.\n', xdriver);
  else
    fprintf('Uses the xf86-video-modesetting DDX video driver.\n');
  end

  % Find Mesa version, if this is running on Mesa.
  mesaVerstr = strfind(winfo.GLVersion, 'Mesa');
  if ~isempty(mesaVerstr)
    mesaVersion = sscanf(winfo.GLVersion(mesaVerstr:end), 'Mesa %i.%i.%i');
  else
    mesaVersion = [0,0,0];
  end

  % Identify major version of Broadcom VideoCore on a RaspberryPi, and deep color caps:
  videoCoreDeepColor = 0;
  if ~isempty(strfind(winfo.GPUCoreId, 'VC4'))
    videoCoreVersion = sscanf(winfo.GLRenderer(end-2:end), '%i.%i');

    % VideoCore 4 or later, on Mesa 23.3.0 or later? Then it is 10 bpc deep color capable:
    if videoCoreVersion(1) >= 4 && (mesaVersion(1) > 23 || (mesaVersion(1) == 23 && mesaVersion(2) >= 3))
      videoCoreDeepColor = 1;
    end
  else
    videoCoreVersion = [];
  end

  % Step 2: Enumerate all available video outputs on all X-Screens:
  outputs = [];
  outputCnt = 0;
  screenNumbers = Screen('Screens');
  for screenNumber = screenNumbers
    % Reject non-existent screens:
    if ~ismember(screenNumber, Screen('Screens'))
      continue;
    end

    % Get number of video outputs per screen:
    numOutputs = Screen('ConfigureDisplay', 'NumberOutputs', screenNumber);
    for outputId = 0:numOutputs-1
      outputCnt = outputCnt + 1;
      outputs{outputCnt} = Screen('ConfigureDisplay', 'Scanout', screenNumber, outputId);
      outputs{outputCnt}.screenNumber = screenNumber;
    end
  end

  fprintf('Found a total of %i video output displays on %i X-Screens.\n\n', outputCnt, length(screenNumbers));

  % HybridGraphics laptop?
  [multigpu, suitable, fullysupported] = DetectHybridGraphics(winfo, xdriver);

  % Single display setup?
  if outputCnt == 1
    % Yes. That answers the question if multi-x-screen is wanted:
    fprintf('Only a single active display connected, so obviously you want a single-display setup.\n');
    multixscreen = 0;
  else
    fprintf('Multiple displays (active video outputs) detected. Do you want a setup with one single\n');
    fprintf('X-Screen or a Dual X-Screen setup? A single X-Screen setup will automatically\n');
    fprintf('adapt to the number and type of connected displays, plug & play. It will also provide the\n');
    fprintf('highest graphics performance, lowest latency, and best timing reliability/precision\n');
    fprintf('for displaying a single Psychtoolbox fullscreen onscreen window for visual stimulation,\n');
    fprintf('because Psychtoolbox would take over exclusive control of your graphics card with its\n');
    fprintf('single onscreen window covering the whole single X-Screen and all its video outputs.\n\n');
    fprintf('For stereoscopic / binocular dual-display stimulation you would just have two stimulation\n');
    fprintf('displays enabled, showing a single Psychtoolbox onscreen window in stereomode 4 or 5.\n');
    fprintf('For single display stimulation you would turn off all displays but the one you want to\n');
    fprintf('use for visual stimulation. Ditto for triple-display stimulation etc.\n');
    fprintf('\n');
    fprintf('The downside of a single X-Screen setup is obviously that you can not display a regular\n');
    fprintf('desktop GUI, e.g., with your Octave or Matlab window, while the experiment session is running.\n');
    fprintf('You will either see the desktop GUI with Octave/Matlab, or the Psychtoolbox window.\n');
    fprintf('\n');
    fprintf('A dual X-Screen "ZaphodHeads" setup on the other hand will allow you to split your graphics cards\n');
    fprintf('displays into two completely separate sets. The video displays assigned to X-Screen 0 will continue\n');
    fprintf('to display your regular desktop GUI with the Octave or Matlab windows and other applications.\n');
    fprintf('The second X-Screen (number 1) will have all the visual stimulation displays attached for\n');
    fprintf('exclusive use by a single Psychtoolbox onscreen window. This will still provide good performance\n');
    fprintf('and the convenience of a visible GUI, although a bit of performance and timing robustness will\n');
    fprintf('have to be sacrificed, because some GPU resources have to be used to drive the desktop GUI.\n');
    fprintf('\n');
    fprintf('You could also create a setup with as many X-Screens as there are outputs available for\n');
    fprintf('special configurations, e.g., stimulation of two or more separate subjects at once.\n');
    fprintf('\n\n');
    answer = '';
    while isempty(answer) || ~ismember(answer, ['s', 'm'])
      answer = input('Do you want a single X-Screen (s) or a dual/multi X-Screen (m) setup? [s / m] ', 's');
    end
    fprintf('\n\n');

    if answer == 'm'
      multixscreen = 1;
    else
      multixscreen = 0;
    end
  end

  % Setup of NoAutoAddGPU needed? Some muxed hybrid graphics laptops
  % need this if both gpus have outputs, but the set of outputs of one
  % of the gpus is free floating if the other one is selected via mux.
  % The ghost outputs then confuse the X-Servers setup of screen layout
  % and one has outputs which occupy x-screen space but can't display
  % anything, causing a deeply confusing desktop layout.
  % NoAutoAddGPU prevents secondary gpus from being added as slave gpus
  % to the X-Screen, thereby their outputs don't show up. Downside would
  % be a regular GUI setup won't be able to access external displays on
  % a dual-gpu setup where only the 2nd gpu can drive external outputs.
  % A dual-x-screen setup won't have that problem though, so a typical
  % visual stimulation setup would not suffer, only regular desktop use.
  noautoaddgpu = 0;
  if multigpu
    answer = '';
    while isempty(answer) || ~ismember(answer, ['y', 'n'])
      answer = input('Do you experience weird issues with display arrangements and want me to try to fix it? [y / n] ', 's');
    end

    if answer == 'y'
      noautoaddgpu = 1;
    end
  end

  if multixscreen == 0
    serverLayout = '';
    xscreencount = 0;
  else
    % Multi-X-Screen. Step through X-Screens and assign ZaphodHeads:
    screenNumber = 0;
    anotherScreen = 1;
    remainingOutputs = 1:outputCnt;
    xscreenoutputs{1} = [];
    while (anotherScreen > 0) && (length(remainingOutputs) > 0)
      % Display windows with Output name labels on each output:
      fprintf('The following outputs are available for assignment to X-Screen %i:\n\n', screenNumber);
      for i=1:outputCnt
        if ismember(i, remainingOutputs)
          scanout = outputs{i};
          winRect = OffsetRect([0, 0, 600, 100], scanout.xStart, scanout.yStart);
          w(i) = Screen('Openwindow', scanout.screenNumber, 0, winRect, 24);
          Screen('TextSize', w(i), 48);
          Screen('DrawText', w(i), [num2str(i) ') ' scanout.name], 10, 10, 255);
          Screen('Flip', w(i));
          fprintf('%i) Output %s.\n', i, scanout.name);
        end
      end
      fprintf('\n');
      fprintf('Please enter the numbers of the outputs which should be assigned to X-Screen %i.\n', screenNumber);
      if screenNumber == 0
        fprintf('These will be the display outputs used to display the GUI with Octave and Matlab.\n');
      else
        fprintf('These will be the display outputs used for visual stimulation.\n');
      end

      outputNumbers = '';
      while isempty(outputNumbers) || ~all(ismember(outputNumbers, remainingOutputs))
        answer = input('Enter space-separated list of numbers of outputs to use, RETURN to finish: ', 's');
        outputNumbers = str2num(answer);
      end

      % Ok, outputNumbers contains the outputs that this screen should use.

      % Remove them from list of unassigned outputs:
      remainingOutputs = setdiff(remainingOutputs, outputNumbers);

      % Add them to list of outputs for this screen:
      xscreenoutputs{screenNumber + 1} = outputNumbers;

      Screen('CloseAll');

      % Setup another screen?
      if length(remainingOutputs) > 0
        % Possible, as there are unassigned outputs:
        fprintf('\nThere are %i unassigned active video outputs left.\n', length(remainingOutputs));
        answer = '';
        while isempty(answer) || ~ismember(answer, ['y', 'n'])
          answer = input('Do you want to create another X-Screen for some of these? [y/n] ', 's');
        end

        if answer == 'n'
          anotherScreen = 0;
        else
          screenNumber = screenNumber + 1;
          xscreenoutputs{screenNumber + 1} = [];
        end
      end
    end

    % Ok, we have a selection for each screen:
    fprintf('\n\n');
    fprintf('Will create %i X-Screens with the following output assignment:\n', screenNumber + 1);
    totalAssignedOutputCnt = 0;
    for i = 0:screenNumber
      fprintf('X-Screen %i: ', i);
      for j=1:length(xscreenoutputs{i + 1})
        scanout = outputs{(xscreenoutputs{i + 1}(j))};
        if j == 1
          ZaphodHeads{i+1} = scanout.name;
        else
          ZaphodHeads{i+1} = [ZaphodHeads{i+1} ',' scanout.name];
        end
        totalAssignedOutputCnt = totalAssignedOutputCnt + 1;
      end
      fprintf('%s\n', ZaphodHeads{i+1});
    end
  end

  % Setup of special driver options?
  answer = '';
  while isempty(answer) || ~ismember(answer, ['y', 'n'])
    answer = input('Do you want to configure special/advanced settings? [y/n] ', 's');
  end

  % Auto-choice of modesetting-ddx or not:
  modesetting = 'd';
  usegamma = -1;
  asyncflipsecondaries = 'd';
  vrhmdondesktop = 0;

  if answer == 'n'
    % Nope. Just use the "don't care" settings:
    depth30bpp = 'd';
    vrrsupport = 'd';

    % Keep using modesetting if it is already in use:
    if modesettingddxactive
      xdriver = 'modesetting';
    end
  else
    % Ask questions for setup of advanced options:

    % Depth 30 is not supported on old Broadcom VideoCore-4 of RaspberryPi 1/2/3, otherwise give it a shot:
    if ~strcmp(winfo.GPUCoreId, 'VC4') || videoCoreDeepColor
      % 10 bpc depth 30 deep color for 1 billion colors wanted? All AMD, Intel and NVidia gpu's support this,
      % both with open-source and proprietary (NVidia) drivers, also with modesetting-ddx. Some SoC gpu's also
      % support it, but not all.
      fprintf('\n\nDo you want to setup a 30 bit framebuffer for 10 bpc precision per color channel?\n');
      fprintf('All AMD, Intel and NVidia gpus since at least 2010, sometimes since 2005 support this,\n');
      fprintf('The RaspberryPi 4 and later support 10 bpc with Linux 5.15 + Mesa 23.3.1 and later.\n');
      fprintf('Some other modern SoC gpus from ARM, Qualcomm and others may support it as well.\n');
      fprintf('If your desktop GUI fails to work, or Psychtoolbox gives lots of timing or page-flip related warnings,\n');
      fprintf('then your system and hardware may not be ready for this depth 30 mode. On AMD hardware sold from the\n');
      fprintf('years 2005 to ~2019, up to and including AMD Polaris, but *not* anymore for AMD Vega, Navi RX 5000 or AMD Ryzen\n');
      fprintf('processor integrated graphics or later models, depth 30 will always work, even without the need to set\n');
      fprintf('it up here, at least for PsychImaging native 10 bit framebuffer tasks, albeit at potentially slightly\n');
      fprintf('lower performance. These slightly older AMD gpus have special support by Psychtoolbox in this sense.\n');
      fprintf('Also note that not all gpus can output true 10 bpc on all types of video outputs. Check carefully with a photometer!\n');

      depth30bpp = '';
      while isempty(depth30bpp) || ~ismember(depth30bpp, ['y', 'n', 'd'])
        depth30bpp = input('Use a 30 bpp, 10 bpc framebuffer [y for yes, n for no, d for don''t care]? ', 's');
      end

      % Some Intel gpus need a force-enable of use of GAMMA_LUT's by the modesetting ddx
      % for deep color mode, because the legacy lut's would only provide insufficient 8 bpc
      % precision, and the high precision GAMMA_LUT's won't get auto-enabled on some gpus.
      % We assume we are running on Linux 5.15 or later, and the Intel gpus requiring this
      % would be Gen-11 Icelake, and all Gen-12 gpus, e.g., Tigerlake, DG-1, Alderlake-S.
      % These are considered broken by X-Server 21 modesetting ddx, but we know they have
      % been fixed as of Linux 5.15 / Ubuntu 22.04, so we force-enable GAMMA_LUT's.
      % usegamma will only have an effect under modesetting ddx, so this only affects
      % Intel gpus under modesetting ddx. And yes, gamma handling on the latest Intel gens
      % is a frickin mess, if you haven't guessed it already...
      if depth30bpp == 'y' && strcmp(xdriver, 'intel')
        usegamma = 1;
      end

      % Depth 30 on multi-x-screen setup requested?
      if depth30bpp == 'y' && multixscreen
        depth30bpp = '';
        while isempty(depth30bpp) || isempty(str2num(depth30bpp)) || ~isnumeric(str2num(depth30bpp))
          depth30bpp = input('Enter a space-separated list of screen numbers for which 30 bit color depth should be used: ', 's');
        end
      end
    else
      % Unsupported gpu - Don't care means no:
      depth30bpp = 'd';
    end

    % Mesa FOSS graphics driver on top of open-source Linux DRM/KMS? And definitely not a gpu+driver combo that can't do VRR?
    if ~isempty(strfind(winfo.GLVersion, 'Mesa')) && ~strcmp(xdriver, 'nouveau') && ~(strcmp(xdriver, 'intel') && ~modesettingddxactive)
      % Possibly VRR capable Mesa driver + Linux DRM/KMS driver:
      fprintf('\n\nDo you want to allow use of so called VRR Variable Refresh Rate Mode?\n');
      fprintf('This is also known as FreeSync or DisplayPort adaptive sync. It allows to control\n');
      fprintf('visual stimulus onset with more fine-grained timing (see ''help VRRSupport'' for more infos).\n');
      fprintf('This currently only works on AMD Sea Islands gpus and later or Intel Gen-11 Icelake gpus and later,\n');
      fprintf('with suitable displays and cables. It also needs at least Linux 5.2 for AMD gpus or at least\n');
      fprintf('Linux 5.12 for Intel Gen-12 Tigerlake gpus and at least Linux 5.17 for Intel Gen-11 Icelake gpus.\n');
      vrrsupport = '';
      while isempty(vrrsupport) || ~ismember(vrrsupport, ['y', 'n', 'd'])
        vrrsupport = input('Allow use of VRR Variable Refresh Rate mode [y for yes, n for no, d for don''t care]? ', 's');
      end
    else
      vrrsupport = 'd';
    end

    if ~strcmp(xdriver, 'nvidia')
      % AsyncFlipSecondaries capable Linux DRM/KMS driver:
      fprintf('\n\nDo you want to allow use of so called AsyncFlipSecondaries Mode?\n');
      fprintf('This takes effect whenever you present a fullscreen onscreen window\n');
      fprintf('on an X-Screen with multiple video outputs / displays connected and\n');
      fprintf('active.\n');
      fprintf('\n');
      fprintf('It is most useful if you have a setup where the visual stimulus is shown\n');
      fprintf('in a mirrored/cloned configuration on both, a display for your subject,\n');
      fprintf('and a "monitoring" display for the experimenter, e.g., a fMRI setup\n');
      fprintf('with a monitor/projector displaying the stimulus to the subject in the\n');
      fprintf('scanner bore, and another control monitor in the control room. If both\n');
      fprintf('displays are not perfectly synchronized, this can cause presentation\n');
      fprintf('timing judder/problems or degraded performance. This option allows you\n');
      fprintf('to get best timing and performance and quality on the subject display,\n');
      fprintf('e.g., the projector in the fMRI scanner, by sacrificing quality for the\n');
      fprintf('experimenter display in the control room, e.g., getting some tearing.\n');
      fprintf('If you have such a setup, enabling this mode may make sense. Timing\n');
      fprintf('will be preserved tear-free for the highest resolution display in a\n');
      fprintf('mirror configuration, or in the case of all displays having the same\n');
      fprintf('resolution, for the user designated primary display.\n');
      fprintf('\n');
      fprintf('If you intend to only use a single display per x-screen, or have perfectly\n');
      fprintf('synchronized displays then this option is useless and answering n or d is best.\n');
      asyncflipsecondaries = '';
      while isempty(asyncflipsecondaries) || ~ismember(asyncflipsecondaries, ['y', 'n', 'd'])
        asyncflipsecondaries = input('Use AsyncFlipSecondaries mode for multi-display setups [y for yes, n for no, d for don''t care]? ', 's');
      end
    end

    if strcmp(xdriver, 'nvidia')
      % Expose VR HMD's on X11 desktop with NVidia proprietary driver? Only useful with
      % PsychOpenHMDVR and PsychOculusVR:
      fprintf('\n\nDo you want to use Virtual Reality HMDs of type Rift-DK1/DK2 or with OpenHMD?\n');
      fprintf('Say yes if you want to drive a Oculus Rift DK1 or DK2, or use other VR HMDs with\n');
      fprintf('the old OpenHMD VR driver, instead of the modern OpenXR driver. This will expose the\n');
      fprintf('HMD as regular desktop display on NVidia graphics, so our old OculusVR and OpenHMD\n');
      fprintf('drivers can use it, preferrably in a multi-X-Screen setup with separate X-Screen for the\n');
      fprintf('HMD. This will disable any meaningful use of Vulkan displays under NVidia, and disable\n');
      fprintf('use of VR HMDs with the modern OpenXR driver, e.g., with Monado or SteamVR.\n');
      fprintf('Answering (n)o or (d)ont care is usually the right choice.\n');
      vrhmdondesktop = '';
      while isempty(vrhmdondesktop) || ~ismember(vrhmdondesktop, ['y', 'n', 'd'])
        vrhmdondesktop = input('Use VR HMD with OpenHMD or OculusVR [y for yes, n for no, d for don''t care]? ', 's');
      end

      if vrhmdondesktop == 'y'
        vrhmdondesktop = 1;
      else
        vrhmdondesktop = 0;
      end
    end

    % Use or non-use of modesetting ddx possible on this gpu hardware?
    if expertmode && ~strcmp(xdriver, 'nvidia') && ~strcmp(xdriver, 'modesetting')
        % Yes: The xf86-video-modesetting driver is an option that supports DRI3/Present well.
        fprintf('\n\nDo you want to use the modesetting driver xf86-video-modesetting?\n');
        fprintf('This is a driver which works in principle with all open-source kernel display drivers.\n');
        fprintf('Your operating system will have made the optimal choice of driver for most conceivable use cases,\n');
        fprintf('so rarely will there be a justified need to override this automatic choice.\n');
        fprintf('Therefore if you do not have a good reason otherwise, just answer d for don''t care as a safe choice.\n');
        if multixscreen && ~modesettingddxactive
          fprintf('CAUTION: When setting up a multi-x-screen setup with modesetting, you must do this in two separate\n');
          fprintf('CAUTION: steps. First run this script followed by XOrgConfSelector to select modesetting in a\n');
          fprintf('CAUTION: single x-screen setup, then logout and login again. Then run XOrgConfCreator again, selecting\n');
          fprintf('CAUTION: a multi-x-screen setup with the modesetting driver selected again. If you do not follow this\n');
          fprintf('CAUTION: order you would end up with a dysfunctional graphical user interface!\n');
          fprintf('CAUTION: If you answer ''yes'' below, i will modify your choice, so you can safely execute\n');
          fprintf('CAUTION: the first step of this procedure, so no worries...\n');
        end

        usemodesetting = '';
        while isempty(usemodesetting) || ~ismember(usemodesetting, ['y', 'n', 'd'])
          usemodesetting = input('Use modesetting driver [y for yes, n for no, d for don''t care - d IS RECOMMENDED]? ', 's');
        end

        % Force-Choose modesetting on explicit yes:
        if usemodesetting == 'y'
          xdriver = 'modesetting';
          modesetting = 'y';
        end

        % If the user explicitly does not want modesetting, and there are no forcing circumstances to use or not use it,
        % (aka modesetting == 'd') and use of another driver is possible, then force modesetting off. As of Ubuntu 20.04,
        % the distro will select modesetting-ddx by default for everything except for AMD gpu's (radeon-ddx/amdgpu-ddx)
        % or NVidia gpu's with proprietary nvidia-ddx, so we need a config file to opt-out of modesetting-ddx, not to opt-in.
        if (usemodesetting == 'n') && (modesetting == 'd') && ~strcmp(xdriver, 'modesetting')
          modesetting = 'n';
        end
    end

    % Map a "Don't care" about modesetting to choice of modesetting if modesetting is already active.
    % We'd do this anyway below in an override, but doing it early allows to skip all those redundant questions below:
    if (modesetting == 'd') && modesettingddxactive
      xdriver = 'modesetting';
    end

    % End of advanced configuration.
  end
catch
  fprintf('Sorry, something went wrong. Aborting. The error message was:\n');

  % Close all windows:
  Screen('CloseAll');

  % Restore old Screen settings:
  Screen('Preference', 'SkipSyncTests', oldSyncTests);
  Screen('Preference', 'Verbosity', oldVerbosity);
  psychrethrow(psychlasterror);
end

fprintf('\n\n');

% Close all windows:
Screen('CloseAll');

% Restore old Screen settings:
Screen('Preference', 'SkipSyncTests', oldSyncTests);
Screen('Preference', 'Verbosity', oldVerbosity);

% We have all information and answers we wanted. Synthesize a xorg.conf:

% Do we need to enforce modesetting-ddx for AsyncFlipSecondaries mode, because the
% gpu is not a modern amdgpu-kms powered AMD, and so can't use amdgpu-ddx, and only
% modesetting-ddx does the AsyncFlipSecondaries trick for such other gpu's?
if asyncflipsecondaries == 'y' && ~strcmp(xdriver, 'amdgpu') && ~strcmp(xdriver, 'amdgpu-pro') && ~strcmp(xdriver, 'modesetting')
  fprintf('Override: AsyncFlipSecondaries mode is requested on something else than a modern AMD gpu.\n');
  fprintf('Override: This requires use of the modesetting-ddx, but modesetting-ddx is not currently\n');
  fprintf('Override: selected as driver. Switching driver from %s-ddx to modesetting-ddx.\n', xdriver);
  fprintf('Override: This may invalidate a multi-x-screen configuration, requiring a two-step switch\n');
  fprintf('Override: with logout, login and rerun of XOrgConfCreator, lets see...\n\n');
  xdriver = 'modesetting';
  modesetting = 'y';
end

% Is a ddx in use which doesn't default to DRI3?
if strcmp(xdriver, 'intel') || strcmp(xdriver, 'nouveau')
  % Yes. Enforce DRI3/Present to get best performance, reliability and also
  % well working PRIME renderoffload for hybrid graphics machines:
  dri3 = 'y';
else
  % No. Nothing to do:
  dri3 = 'd';
end

% Actually any xorg.conf for non-standard settings needed?
if noautoaddgpu == 0 && multixscreen == 0 && dri3 == 'd' && modesetting == 'd' && ...
   ~isempty(intersect(depth30bpp, 'nd')) && ~strcmp(xdriver, 'nvidia') && vrrsupport ~= 'y' && ...
   usegamma == -1 && asyncflipsecondaries ~= 'y' && ~IsARM

  % All settings are for a single X-Screen setup with auto-detected outputs
  % and all driver settings on default and not on a NVidia proprietary driver.
  % There isn't any need or purpose for a xorg.conf file, so we are done.
  fprintf('With the settings you requested, there is no need for a xorg.conf file at all,\n');
  fprintf('so i will not create one and we are done. Bye!\n\n');
  return;
end

% Multi X-Screen ZaphodHeads setup defined while modesetting-ddx was active, but user does
% not want modesetting-ddx? Or vice versa user wants modesetting-ddx but ZaphodHeads was
% created while a different ddx was active? In those cases, the determined ZaphodHead output
% names would be wrong for the chosen ddx, resulting in X-Server startup failure!
% Therefore, disable multi-x-screen ZaphodHeads and ask user for a two-step transition instead,
% so user is not left with a dysfunctional multi x-screen setup:
if (multixscreen > 0) && (modesetting == 'n') && modesettingddxactive
  multixscreen = 0;
  fprintf('Override: Ignoring request for multi X-Screen configuration, as modesetting-ddx\n');
  fprintf('Override: is not wanted, but was active while creating this config. This would\n');
  fprintf('Override: create an invalid configuration. Generating a single-x-screen config now\n');
  fprintf('Override: without modesetting-ddx. Please repeat the multi-x-screen setup without modesetting,\n');
  fprintf('Override: after logging out and in again with this new configuration selected.\n');
elseif (multixscreen > 0) && (modesetting == 'y') && ~modesettingddxactive
  multixscreen = 0;
  fprintf('Override: Ignoring request for multi X-Screen configuration, as modesetting-ddx\n');
  fprintf('Override: is requested, but was not active while creating this config. This would\n');
  fprintf('Override: create an invalid configuration. Generating a single-x-screen modesetting\n');
  fprintf('Override: config now. Please repeat the multi-x-screen + modesetting setup after logging\n');
  fprintf('Override: out and in again with this new configuration selected.\n');
end

% Define filename of output file:
fdir = PsychtoolboxConfigDir ('XorgConfs');

if multixscreen > 0
  fname = [fdir sprintf('90-ptbconfig_%i_xscreens_%i_outputs_%s.conf', screenNumber+1, totalAssignedOutputCnt, xdriver)];
else
  fname = [fdir sprintf('90-ptbconfig_single_xscreen_%s.conf', xdriver)];
end

fprintf('Ready to write the config file. I propose this filename and location:\n');
fprintf('%s\n', fname);
answer = input('Type ENTER/RETURN to accept, or provide an alternative filename: ', 's');
if ~isempty(answer)
  fname = answer;
  fprintf('Will store at this location instead:\n%s\n\n', fname);
end

% Need a xorg.conf file, so create one:
[fid, errmsg] = fopen(fname, 'w');
if fid == -1
  fprintf('ERROR: Could not create xorg.conf output file "%s".\n', fname);
  fprintf('ERROR: The OS said: %s\n', errmsg);
  error('Game over.');
end

% Header:
fprintf(fid, '# Auto generated xorg.conf - Created by Psychtoolbox XOrgConfCreator.\n\n');

if noautoaddgpu > 0
  fprintf(fid, 'Section "ServerFlags"\n');
  if noautoaddgpu
    fprintf(fid, '  Option "AutoAddGPU"     "false"\n');
  end

  fprintf(fid, 'EndSection\n\n');
end

if multixscreen == 0 && dri3 == 'd' && modesetting == 'd' && asyncflipsecondaries ~= 'y' && ...
   ~isempty(intersect(depth30bpp, 'nd')) && (~strcmp(xdriver, 'nvidia') || ~vrhmdondesktop) && ...
   vrrsupport ~= 'y' && usegamma == -1 && ~IsARM
  % Done writing the file:
  fclose(fid);
else
  % Multi X-Screen setup requested? Then we need the full show:
  if multixscreen > 0
    % General server layout: Which X-Screens to use, their relative
    % spatial location wrt. each other:
    fprintf(fid, 'Section "ServerLayout"\n');
    fprintf(fid, '  Identifier    "PTB-Hydra"\n');
    fprintf(fid, '  Screen 0      "Screen0" 0 0\n');
    for i = 1:screenNumber
      fprintf(fid, '  Screen %i      "Screen%i" RightOf "Screen%i"\n', i, i, i-1);
    end
    fprintf(fid, 'EndSection\n\n');

    % Monitor sections, one for each assigned video output. Monitor's within
    % a given X-Screen are ordered relative to each other left to right in the
    % order their corresponding ZaphodHeads outputs were defined by the user:
    for i = 0:screenNumber
      for j=1:length(xscreenoutputs{i + 1})
        scanout = outputs{(xscreenoutputs{i + 1}(j))};
        fprintf(fid, 'Section "Monitor"\n');
        fprintf(fid, '  Identifier    "%s"\n', scanout.name);
        if j > 1
          scanout = outputs{(xscreenoutputs{i + 1}(j-1))};
          fprintf(fid, '  Option        "RightOf" "%s"\n', scanout.name);
        end
        fprintf(fid, 'EndSection\n\n');
      end
    end

    % Create device sections, one for each x-screen aka the driver instance
    % associated with that x-screen:
    for i = 0:screenNumber
      WriteGPUDeviceSection(fid, xdriver, usegamma, asyncflipsecondaries, vrrsupport, dri3, i, ZaphodHeads{i+1}, xscreenoutputs{i+1}, outputs);
    end

    % One screen section per x-screen, mapping screen i to card i:
    for i = 0:screenNumber
      fprintf(fid, 'Section "Screen"\n');
      fprintf(fid, '  Identifier    "Screen%i"\n', i);
      fprintf(fid, '  Device        "Card%i"\n', i);
      % If there's exactly one ZaphodHead for this screen then
      % explicitly assign the Monitor id here. This is also done
      % in the Device section per ZaphodHead, and doing it here
      % as well doesn't make sense to me, but that's what is
      % mysteriously needed on XUbuntu 16.04, according to a user
      % report. Lets hope this only helps but doesn't hurt.
      if length(xscreenoutputs{i + 1}) == 1
        scanout = outputs{(xscreenoutputs{i + 1}(1))};
        fprintf(fid, '  Monitor       "%s"\n', scanout.name);
      end

      if ismember('y', depth30bpp) || ismember(num2str(i), depth30bpp)
        fprintf(fid, '  DefaultDepth  30\n');
      end

      % On NVidia ddx, optionally tell the driver to also expose HMD's, e.g.,
      % for use with our OpenHMD VR driver, instead of hiding them. This will
      % disable Vulkan direct display mode and modern OpenXR:
      if strcmp(xdriver, 'nvidia') && vrhmdondesktop
        fprintf(fid, '  Option "AllowHMD"         "yes"\n');
      end

      fprintf(fid, 'EndSection\n\n');
    end
  else
    % Only a single X-Screen.

    % We only need to create a single device section with override Option
    % values for the gpu driving that single X-Screen.
    WriteGPUDeviceSection(fid, xdriver, usegamma, asyncflipsecondaries, vrrsupport, dri3, [], [], []);

    if ismember('y', depth30bpp) || ismember('0', depth30bpp) || strcmp(xdriver, 'nvidia') || ...
       (strcmp(xdriver, 'modesetting') && IsARM)
      fprintf(fid, 'Section "Screen"\n');
      fprintf(fid, '  Identifier    "Screen%i"\n', 0);
      fprintf(fid, '  Device        "Card%i"\n', 0);

      if ismember('y', depth30bpp) || ismember('0', depth30bpp)
        fprintf(fid, '  DefaultDepth  30\n');
      end

      % On NVidia ddx, optionally tell the driver to also expose HMD's, e.g.,
      % for use with our OpenHMD VR driver, instead of hiding them. This will
      % disable Vulkan direct display mode and modern OpenXR:
      if strcmp(xdriver, 'nvidia') && vrhmdondesktop
        fprintf(fid, '  Option "AllowHMD"         "yes"\n');
      end

      fprintf(fid, 'EndSection\n\n');
    end
  end

  % Done writing the file:
  fclose(fid);
end

fprintf('\n\nWe are done. Now you can run XOrgConfSelector any time to select this configuration\n');
fprintf('file to setup your system, or to switch back to the default setup of your system. Bye!\n\n');

end

function WriteGPUDeviceSection(fid, xdriver, usegamma, asyncflipsecondaries, vrrsupport, dri3, screenNumber, ZaphodHeads, xscreenoutputs, outputs)
  fprintf(fid, 'Section "Device"\n');

  if isempty(screenNumber)
    fprintf(fid, '  Identifier  "Card0"\n');
  else
    fprintf(fid, '  Identifier  "Card%i"\n', screenNumber);
  end

  % Override our label with actual ddx module name:
  if strcmp(xdriver, 'amdgpu-pro')
    xdriver = 'amdgpu';
  end

  fprintf(fid, '  Driver      "%s"\n', xdriver);

  if vrrsupport ~= 'd'
    if vrrsupport == 'y'
      vrrsupport = 'on';
    else
      vrrsupport = 'off';
    end
    fprintf(fid, '  Option      "VariableRefresh" "%s"\n', vrrsupport);
  end

  if asyncflipsecondaries ~= 'd'
    if asyncflipsecondaries == 'y'
      asyncflipsecondaries = 'on';
    else
      asyncflipsecondaries = 'off';
    end
    fprintf(fid, '  Option      "AsyncFlipSecondaries" "%s"\n', asyncflipsecondaries);
  end

  if dri3 ~= 'd'
    if dri3 == 'y'
      dri3 = '3';
    else
      dri3 = '2';
    end
    fprintf(fid, '  Option      "DRI" "%s"\n', dri3);
  end

  % RaspberryPi with VideoCore4 needs AccelMethod override to glamor, because
  % RaspberryPi OS 11, as of late 2022, disables glamor, and thereby any
  % hardware acceleration for OpenGL on VideoCore4 to conserve RAM, something
  % that doesn't fly with our use case:
  if strcmp(xdriver, 'modesetting') && IsARM
    fprintf(fid, '  Option      "AccelMethod" "glamor"\n');
  end

  % modesetting ddx in use and explicit UseGammaLUT setup wanted?
  if strcmp(xdriver, 'modesetting') && (usegamma ~= -1)
    if usegamma == 1
      fprintf(fid, '  Option      "UseGammaLUT" "on"\n');
    else
      fprintf(fid, '  Option      "UseGammaLUT" "off"\n');
    end
  end

  if ~isempty(screenNumber)
    if ~isempty(ZaphodHeads)
      if strcmp(xdriver, 'nvidia')
        fprintf(fid, '  Option      "UseDisplayDevice" "%s"\n', ZaphodHeads);
      else
        fprintf(fid, '  Option      "ZaphodHeads" "%s"\n', ZaphodHeads);
      end

      for i=1:length(xscreenoutputs)
        scanout = outputs{(xscreenoutputs(i))};
        fprintf(fid, '  Option      "Monitor-%s" "%s"\n', scanout.name, scanout.name);
      end

    end
    fprintf(fid, '  Screen %i\n', screenNumber);
  end

  fprintf(fid, 'EndSection\n\n');
end

function xdriver = DetectDDX(winfo)
  if ~isempty(strfind(winfo.DisplayCoreId, 'Intel'))
    % Intel part -> intel ddx:
    fprintf('Intel GPU detected. ');
    xdriver = 'intel';
  elseif ~isempty(strfind(winfo.DisplayCoreId, 'NVidia')) && ~isempty(strfind(winfo.GLVendor, 'nouveau'))
    % NVidia part under nouveau -> nouveau ddx:
    fprintf('Nvidia GPU with open-source driver detected. ');
    xdriver = 'nouveau';
  elseif ~isempty(strfind(winfo.DisplayCoreId, 'NVidia')) && ~isempty(strfind(winfo.GLVendor, 'NVIDIA'))
    % NVidia part under binary blob -> nvidia ddx:
    fprintf('Nvidia GPU with proprietary driver detected. ');
    xdriver = 'nvidia';
  elseif ~isempty(strfind(winfo.DisplayCoreId, 'AMD'))
    % Some AMD/ATI Radeon/Fire series part:
    if ~isempty(strfind(winfo.GLVersion, 'Mesa'))
      % Controlled by the open-source drivers. GPU minor
      % type defines the DCE generation and that in turn
      % usually predicts the x driver well:
      if winfo.GPUMinorType >= 100 || winfo.GPUMinorType == -1
        % DCE-10 or later, ergo "Volcanic Islands" family or later. Or a brand-new
        % DCN class gpu (Ryzen+ APU's, Navi+, ...) or later -> amdgpu ddx:
        fprintf('Recent AMD GPU with open-source driver detected. ');
        xdriver = 'amdgpu';
      elseif (winfo.GPUMinorType < 100 && winfo.GPUMinorType >= 80) && exist('/sys/module/amdgpu/parameters/cik_support', 'file')
        % DCE-8 and amdgpu-kms loaded. Is it used for DCE-8 Sea Islands (cik)?
        [rc, msg] = system('cat /sys/module/amdgpu/parameters/cik_support');
        if rc == 0 && length(msg) >= 1 && msg(1) == '1'
          % Seems amdgpu-kms is driving the gpu, ergo amdgpu-ddx is in use!
          fprintf('Sea Islands AMD GPU with open-source amdgpu-kms driver detected. ');
          xdriver = 'amdgpu';
        else
          % amdgpu-kms not in the drivers seat - radeon-kms and ergo ati/radeon-ddx is in use:
          fprintf('Sea Islands AMD GPU with open-source radeon-kms driver detected. ');
          xdriver = 'ati';
        end
      else
        % DCE-8 or earlier under radeon-kms => ati ddx:
        fprintf('Classic AMD GPU with open-source radeon-kms driver detected. ');
        xdriver = 'ati';
      end
    else
      fprintf('AMD GPU with hybrid free+proprietary amdgpu-pro driver detected. ');
      xdriver = 'amdgpu-pro';
    end
  else
    % Warn if we use modesetting ddx because we can not identify gpu, otherwise
    % we still use modesetting ddx for known gpu's if we end here, but we don't
    % warn about it because we know this is the right ddx for those known gpus:
    if ~strcmp(winfo.GPUCoreId, 'VC4')
      warning('Could not identify your graphics driver. Will use modesetting driver as fallback.');
      fprintf('GPU with unknown driver detected. ');
    end

    xdriver = 'modesetting';
  end
end

function [multigpu, suitable, fullysupported] = DetectHybridGraphics(winfo, xdriver)
  % Be pessimistic to start with:
  suitable = 0;
  fullysupported = 0;
  multigpu = 0;

  % Does this machine have multiple gpu's, e.g., hybrid graphics laptop?
  if IsOctave && ~exist('/dev/dri/card1','file')
    % Nope:
    return;
  end

  % Multi-gpu check for Matlab, because Matlab exist() can not check device files:
  if ~IsOctave
    [rc, ~] = system('stat /dev/dri/card1');
    if rc == 1
      % Nope:
      return;
    end
  end

  % Yes, likely a hybrid graphics laptop:
  multigpu = 1;

  % Display gpu not Intel or AMD?
  if ~strcmp(xdriver, 'intel') && ~strcmp(xdriver, 'amdgpu') && ~strcmp(xdriver, 'amdgpu-pro') && ~strcmp(xdriver, 'ati')
    % Then the display gpu must be Nvidia or such.
    % Confirm that primary and secondary gpu are from the same vendor to match that
    % pattern, otherwise we might be dealing with a mux'ed laptop currently switched
    % to the discrete NVidia or unknown gpu:
    if ~exist('/sys/class/drm/card0/device/vendor','file') || ~exist('/sys/class/drm/card1/device/vendor','file')
      return;
    end

    [status, out] = system ('diff /sys/class/drm/card0/device/vendor /sys/class/drm/card1/device/vendor');
    if status ~= 0 || ~isempty(out)
      % Mismatched gpu vendors. Play it safe and assume this is not muxless Optimus or Enduro:
      return;
    end
  end

  fprintf('This seems to be a hybrid graphics laptop.\n');

  c = Screen('Computer');
  osrelease = sscanf(c.kern.osrelease, '%i.%i');

  % Primary display gpu is Intel?
  if strcmp(xdriver, 'intel')
    % These work well with both NVidia (Optimus) and AMD (Enduro) discrete
    % gpu's as of X-Server 1.18, Mesa with well working DRI3/Present support,
    % and Linux kernel 4.5 and later.
    if ~(osrelease(1) > 4 || (osrelease(1) == 4 && osrelease(2) >=5))
      % Kernel too old to provide proper PRIME sync:
      fprintf('Your Linux kernel %s is too old to support proper hybrid graphics with Intel graphics chips.\n', c.kern.osrelease);
      fprintf('However, upgrading an Ubuntu Linux flavor or derivative to a suitable 4.5+ kernel is trivial.\n');
      fprintf('Some small subset of hybrid graphics laptops would allow you to utilize the powerful gpu\n');
      fprintf('in a dual X-Screen configuration for the external video outputs nonetheless, although this\n');
      fprintf('assistant can not help you with setting them up.\n');
      fprintf('Check ''help HybridGraphics'' and the Psychtoolbox website for instructions, follow them, then retry.\n\n');
      return;
    else
      suitable = 1;
      fullysupported = 1;
    end
  end

  % Primary display gpu is modern AMD?
  if strcmp(xdriver, 'amdgpu') || strcmp(xdriver, 'amdgpu-pro')
    % These work well with other AMD discrete gpu's as of X-Server 21 and Linux kernel 5.11 and later.
    if ~(osrelease(1) > 5 || (osrelease(1) == 5 && osrelease(2) >= 11))
      % Kernel too old to provide proper PRIME sync:
      fprintf('Your Linux kernel %s is too old to support proper hybrid graphics with AMD iGPU graphics chips.\n', c.kern.osrelease);
      fprintf('However, upgrading an Ubuntu Linux flavor or derivative to a suitable 5.11+ kernel is trivial.\n');
      fprintf('Some small subset of hybrid graphics laptops would allow you to utilize the powerful gpu\n');
      fprintf('in a dual X-Screen configuration for the external video outputs nonetheless, although this\n');
      fprintf('assistant can not help you with setting them up.\n');
      fprintf('Check ''help HybridGraphics'' and the Psychtoolbox website for instructions, follow them, then retry.\n\n');
      return;
    else
      suitable = 1;
      if exist('/sys/class/drm/card0/device/vendor','file') && exist('/sys/class/drm/card1/device/vendor','file')
        [status, out] = system ('diff /sys/class/drm/card0/device/vendor /sys/class/drm/card1/device/vendor');
        if status == 0 && isempty(out)
          % Same gpu vendor for iGPU and dGPU, both AMD. Pageflipping for PRIME should work.
          fullysupported = 1;
        end
      end
    end
  end

  if suitable
    % DRI3/Present PRIME render offloading is possible on this setup.
    fprintf('This hardware + software setup should allow use of the discrete gpu for faster rendering.\n');
    fprintf('Enabling DRI3/Present support is needed for that, and I will do that for you.\n');
    if fullysupported
      fprintf('Most likely this setup will allow direct pageflipping for great timing and performance.\n');
    else
      fprintf('You will probably not achieve reliable timing or timestamping though. See runtime output\n');
      fprintf('relating to use of NETWM timing mode under GNOME or Ubuntu desktop for possible workarounds.\n');
    end
    fprintf('You will also need to setup your system to use the powerful gpu with Matlab or Octave.\n');
    fprintf('Check ''help HybridGraphics'' and the Psychtoolbox website for instructions, how to do that.\n');
    fprintf('While with most modern hybrid graphics laptops it should then just work, some more exotic\n');
    fprintf('models may still not be able to output anything with good timing to external video outputs.\n');
    fprintf('I can not detect myself if your laptop is such an exotic machine or not, so you will have to try.\n');
    fprintf('Such models can then be set up in a dual-X-Screen configuration to make them work with\n');
    fprintf('research grade timing, but this assistant can not set them up automatically, so you would\n');
    fprintf('have to do it manually. See ''help HybridGraphics'' and the Psychtoolbox website for instructions.\n\n');
    return;
  end

  return;
end
