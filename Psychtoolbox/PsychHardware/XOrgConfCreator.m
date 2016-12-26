function XOrgConfCreator
% XOrgConfCreator - Automatically create X11 config files.
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

% History:
% 04-Nov-2015  mk  Written.
% 25-Apr-2016  mk  Add support for selection of modesetting ddx on XOrg 1.18+
% 28-Aug-2016  mk  Add basic hybrid graphics setup support.

clc;

if ~IsLinux
  fprintf('This function is only supported or useful on Linux. Bye!\n');
  return;
end

if IsWayland
  fprintf('This function is only supported or useful on Linux with the good old X11/XServer stack, not on Wayland. Bye!\n');
  return;
end

% Step 1: Get the currently active display gpu and derive the required
% X video driver from it. We do this by opening a little invisble
% onscreen window, then querying its window info to get the type
% of primary display gpu used:
fprintf('Detecting type of graphics card (GPU) and driver to use...\n');
oldVerbosity = Screen('Preference', 'Verbosity', 1);
oldSyncTests = Screen('Preference', 'SkipSyncTests', 2);

try
  win = Screen('OpenWindow', 0, 0, [0 0 32 32]);
  winfo = Screen('GetWindowInfo', win);
  Screen('CloseAll');

  xdriver = DetectDDX(winfo);
  fprintf('Will use the xf86-video-%s DDX video driver.\n', xdriver);

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
      % disp(outputs{outputCnt});
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
  % to the X-Screen, thereby there outputs don't show up. Downside would
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

  if multigpu && suitable
    answer = '';
    while isempty(answer) || ~ismember(answer, ['y', 'n'])
      answer = input('Do you want to make use of the high-performance secondary discrete gpu (Optimus/Enduro)? [y / n] ', 's');
    end

    % Treat a "no" as if the laptop does not have multiple gpus:
    if answer == 'n'
      multigpu = 0;
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
          w(i) = Screen('Openwindow', scanout.screenNumber, 0, winRect);
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

  if answer == 'n'
    % Nope. Just use the "don't care" settings:
    triplebuffer = 'd';
    useuxa = 'd';
    dri3 = 'd';
    modesetting = 'd';
  else
    % Ask questions for setup of advanced options:
    modesetting = 'd';

    % Which X-Server version is in use?
    [rc, text] = system('xdpyinfo | grep ''X.Org version''');
    if (rc == 0) && ~strcmp(xdriver, 'nvidia') && ~strcmp(xdriver, 'fglrx') && ~strcmp(xdriver, 'modesetting') && ...
       (~multigpu || (~strcmp(xdriver, 'intel') && ~strcmp(xdriver, 'ati')))
      % HybridGraphics Intel + NVidia/AMD needs intel-ddx, modesetting won't work. Ditto for AMD + AMD.
      % XOrg 1.18.0 or later? xf86-video-modesetting is only good enough for our purposes on 1.18 and later.
      % Also must be a Mesa version safe for use with DRI3/Present:
      xversion = sscanf (text, 'X.Org version: %d.%d.%d');
      if (xversion(1) > 1 || (xversion(1) == 1 && xversion(2) >= 18)) && ...
         ~isempty(strfind(winfo.GLVersion, 'Mesa')) && (bitand(winfo.SpecialFlags, 2^24) > 0)
        % Yes: The xf86-video-modesetting driver is an option that supports DRI3/Present well.
        fprintf('\n\nDo you want to use the new kms modesetting driver xf86-video-modesetting?\n');
        fprintf('This is a new video driver, which works with all open-source display drivers.\n');
        fprintf('It is shown to be rather efficient, but not as feature rich and well tested as other drivers yet.\n');
        fprintf('If you are not sure what to select, answer n for no as a safe choice.\n');
        if multixscreen
          fprintf('CAUTION: When setting up a multi-x-screen setup with modesetting, you must do this in two separate\n');
          fprintf('CAUTION: steps. First run this script followed by XOrgConfSelector to select modesetting in a\n');
          fprintf('CAUTION: single x-screen setup, then logout and login again. Then run XOrgConfCreator again, selecting\n');
          fprintf('CAUTION: a multi-x-screen setup with the modesetting driver selected again. If you do not follow this\n');
          fprintf('CAUTION: order you may end up with a dysfunctional graphical user interface!\n');
        end
        usemodesetting = '';
        while isempty(usemodesetting) || ~ismember(usemodesetting, ['y', 'n', 'd'])
          usemodesetting = input('Use modesetting driver [y for yes, n for no, d for don''t care]? ', 's');
        end

        % Only choose modesetting on explicit yes for now:
        if usemodesetting == 'y'
          xdriver = 'modesetting';
          modesetting = 'y';
        end
      end
    end

    if strcmp(xdriver, 'intel') || strcmp(xdriver, 'nouveau')
      fprintf('\n\nDo you want to allow the use of triple-buffering under DRI2?\n');
      fprintf('Triple buffering can potentially cause a slight increase in performance for very\n');
      fprintf('demanding visual stimulation paradigms. However, it is not without downsides. It can\n');
      fprintf('cause visual glitches in some applications other than Psychtoolbox, or for visual\n');
      fprintf('desktop animations, ie. during regular desktop use of your computer.\n');
      fprintf('Also for some stimulation paradigms performance is not consistently increased, but\n');
      fprintf('can become somewhat erratic.\n');
      if strcmp(xdriver, 'intel')
        fprintf('Additionally on some intel graphics drivers with sna acceleration, some uses of\n');
        fprintf('triple-buffering can cause hangs of Psychtoolbox.\n');
      end
      fprintf('To take advantage of the potential performance improvements one also needs to adapt\n');
      fprintf('experiment scripts in a special way, which makes the code only work on Linux, not on OSX or Windows.\n');
      fprintf('Due to this mixed bag of advantages and disadvantages, it is usually better to not\n');
      fprintf('use triple-buffering but instead enable DRI3/Present support if possible for especially\n');
      fprintf('demanding paradigms.');
      fprintf('If you are unsure, or generally happy with the graphics performance, just answer\n');
      fprintf('"d" for "Don''t care", so we leave the decision of what is best to your system.\n');
      fprintf('To try it out, e.g., to get a bit more performance, answer "y", otherwise "n".\n\n');

      triplebuffer = '';
      while isempty(triplebuffer) || ~ismember(triplebuffer, ['y', 'n', 'd'])
        triplebuffer = input('Allow use of DRI2 triple-buffering [y for yes, n for no, d for don''t care]? ', 's');
      end
    else
      triplebuffer = 'd';
    end

    % Allow use of UXA only for non Prime renderoffload, as UXA isn't tested with it so far.
    if strcmp(xdriver, 'intel') && ~multigpu
      fprintf('\n\nShould the alternative Intel display acceleration backend "uxa" be used instead of\n');
      fprintf('the default "sna" backend? Usually "sna" is a fine choice, but this allows you\n');
      fprintf('to choose the alternative backend as a backup plan, if you suspect "sna" causing any\n');
      fprintf('problems. If unsure, just answer "n" for no, it is almost always the correct answer.\n\n');
      useuxa = '';
      while isempty(useuxa) || ~ismember(useuxa, ['y', 'n', 'd'])
        useuxa = input('Use Intel UXA acceleration [y for yes, n for no, d for don''t care]? ', 's');
      end
    else
      useuxa = 'd';
    end

    % Is the use of DRI3/Present safely possible with this combo of Mesa and X-Server?
    if ~strcmp(xdriver, 'modesetting') && ~isempty(strfind(winfo.GLVersion, 'Mesa')) && (bitand(winfo.SpecialFlags, 2^24) > 0)
      % Yes. Propose it:
      fprintf('\n\nDo you want to allow the use of the new DRI3/Present display backend?\n');
      fprintf('DRI3 is a new method of displaying content which is potentially more efficient\n');
      fprintf('and provides potentially higher performance. However, DRI3/Present needs fairly\n');
      fprintf('recent display drivers, otherwise some glitches may occur with this new technology.\n');
      fprintf('\n');
      fprintf('If you are unsure, but generally happy with the graphics performance, just answer\n');
      fprintf('"d" for "Don''t care", so we leave the decision what is best to your system.\n');
      fprintf('To try it out, e.g., to get a bit more performance, answer "y". Psychtoolbox will\n');
      fprintf('warn you during the next session if it thinks you enabled DRI3 on a unsuitable system,\n');
      fprintf('so don''t worry about wrong answers, they can be corrected.\n\n');

      dri3 = '';
      while isempty(dri3) || ~ismember(dri3, ['y', 'n', 'd'])
        dri3 = input('Allow use of DRI3/Present [y for yes, n for no, d for don''t care]? ', 's');
      end
    else
      dri3 = 'd';
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

% Hybrid graphics use requested by user?
if multigpu && suitable && dri3 ~= 'y'
  % Override use of dri3 to 'y'es, as we need DRI3/Present for renderoffload:
  dri3 = 'y';
  fprintf('Override: Enabling DRI3/Present, as this is needed for hybrid graphics Optimus/Enduro support.\n');
end

primehacks = 0;
if multigpu && suitable && ~fullysupported
  primehacks = 1;
end

% Actually any xorg.conf for non-standard settings needed?
if noautoaddgpu == 0 && multixscreen == 0 && dri3 == 'd' && ismember(useuxa, ['d', 'n']) && triplebuffer == 'd' && modesetting == 'd'
  % All settings are for a single X-Screen setup with auto-detected outputs
  % and all driver settings on default. There isn't any need or purpose for
  % a xorg.conf file, so we are done.
  fprintf('With the settings you requested, there is no need for a xorg.conf file at all,\n');
  fprintf('so i will not create one and we are done. Bye!\n\n');
  return;
end

if strcmp(xdriver, 'nvidia')
  fprintf('\n\nCAUTION! You are using the proprietary NVIDIA graphics driver. This script\n');
  fprintf('is not well suited to deal with that driver and may well write an invalid\n');
  fprintf('configuration file! You may want to abort here via pressing CTRL+C and use\n');
  fprintf('NVIDIAs GUI setup tool instead as a more fool proof and safe option.\n');
  fprintf('------------------------ CAUTION -----------------------------------------\n\n');
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
  fprintf(fid, '  Option "AutoAddGPU"     "false"\n');
  fprintf(fid, 'EndSection\n\n');
end

if multixscreen == 0 && dri3 == 'd' && ismember(useuxa, ['d', 'n']) && triplebuffer == 'd' && modesetting == 'd'
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
      WriteGPUDeviceSection(fid, xdriver, dri3, triplebuffer, useuxa, primehacks, i, ZaphodHeads{i+1}, xscreenoutputs{i+1}, outputs);
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
      fprintf(fid, 'EndSection\n\n');
    end
  else
    % Only a single X-Screen. We only need to create a single device
    % section with override Option values for the gpu driving that
    % single X-Screen.
    WriteGPUDeviceSection(fid, xdriver, dri3, triplebuffer, useuxa, primehacks, [], [], []);
  end

  % Done writing the file:
  fclose(fid);
end

fprintf('\n\nWe are done. Now you can run XOrgConfSelector any time to select this configuration\n');
fprintf('file to setup your system, or to switch back to the default setup of your system. Bye!\n\n');

end

function WriteGPUDeviceSection(fid, xdriver, dri3, triplebuffer, useuxa, primehacks, screenNumber, ZaphodHeads, xscreenoutputs, outputs)
  fprintf(fid, 'Section "Device"\n');

  if isempty(screenNumber)
    fprintf(fid, '  Identifier  "Card0"\n');
  else
    fprintf(fid, '  Identifier  "Card%i"\n', screenNumber);
  end

  fprintf(fid, '  Driver      "%s"\n', xdriver);

  if triplebuffer ~= 'd'
    if strcmp(xdriver, 'intel')
      if triplebuffer == 'y'
        triplebuffer = 'on';
      else
        triplebuffer = 'off';
      end
      fprintf(fid, '  Option      "TripleBuffer" "%s"\n', triplebuffer);
    end

    if strcmp(xdriver, 'nouveau')
      if triplebuffer == 'y'
        triplebuffer = '2';
      else
        triplebuffer = '1';
      end
      fprintf(fid, '  Option      "SwapLimit" "%s"\n', triplebuffer);
    end
  end

  if dri3 ~= 'd'
    if dri3 == 'y'
      dri3 = '3';
    else
      dri3 = '2';
    end
    fprintf(fid, '  Option      "DRI" "%s"\n', dri3);
  end

  if (useuxa ~= 'd') && strcmp(xdriver, 'intel')
    if useuxa == 'y'
      useuxa = 'uxa';
    else
      useuxa = 'sna';
    end
    fprintf(fid, '  Option      "AccelMethod" "%s"\n', useuxa);
  end

  if strcmp(xdriver, 'ati') && primehacks
    % The ati ddx for old radeon-kms driven AMD cards allows to disable color tiling
    % for the scanout buffer, and that allows us to get good Enduro hybrid graphics
    % renderoffload on dual-AMD (APU iGPU + AMD dGPU) iff a special hacked kernel is
    % used. This is a dirty trick as a stop-gap measure until we have something well
    % working and clean upstream.
    fprintf(fid, '  Option      "ColorTiling"   "off"\n');
    fprintf(fid, '  Option      "ColorTiling2D" "off"\n');
    fprintf('Enabling special dirty hacks for hybrid graphics Enduro support on legacy AMD+AMD laptops.\n');
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
      if winfo.GPUMinorType >= 100
        % DCE-10 or later, ergo "Volcanic Islands" family or
        % later -> amdgpu ddx:
        fprintf('Recent AMD GPU with open-source driver detected. ');
        xdriver = 'amdgpu';
      else
        % DCE-8 or earlier -> ati ddx:
        fprintf('Classic AMD GPU with open-source driver detected. ');
        xdriver = 'ati';
      end
    else
      % Controlled by Catalyst -> fglrx ddx:
      fprintf('AMD GPU with proprietary Catalyst driver detected. ');
      xdriver = 'fglrx';
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
  if ~exist('/dev/dri/card1','file')
    % Nope:
    return;
  end

  % Yes, likely a hybrid graphics laptop:
  multigpu = 1;

  % Display gpu not Intel?
  if ~strcmp(xdriver, 'intel')
    % Then the display gpu must be Nvidia or AMD. Existing hybrid graphics
    % laptops will have either NVidia + NVidia (rarely) or AMD + AMD (more often).
    % Confirm that primary and secondary gpu are from the same vendor to match that
    % pattern, otherwise we might be dealing with a mux'ed laptop currently switched
    % to the discrete NVidia or AMD gpu:
    if ~exist('/sys/class/drm/card0/device/vendor','file') || ~exist('/sys/class/drm/card1/device/vendor','file')
      return;
    end

    [status, out] = system ('diff /sys/class/drm/card0/device/vendor /sys/class/drm/card1/device/vendor');
    if status ~= 0 || ~isempty(out)
      % Mismatched gpu vendors. Play it safe and assume this is not muxless Optimus or Enduro:
      return;
    end
  end

  fprintf('This seems to be a hybrid graphics laptop. Let''s check if i can use this.\n');

  c = Screen('Computer');
  osrelease = sscanf(c.kern.osrelease, '%i.%i');

  % Mesa and X-Server safe to use for DRI3/Present Prime render offloading?
  primeuseable = ~isempty(strfind(winfo.GLVersion, 'Mesa')) && (bitand(winfo.SpecialFlags, 2^24) > 0);

  % Primary display gpu is Intel?
  if strcmp(xdriver, 'intel')
    % These work well with both NVidia (Optimus) and AMD (Enduro) discrete
    % gpu's as of X-Server 1.18, Mesa with well working DRI3/Present support,
    % and Linux kernel 4.5 and later.
    if ~(osrelease(1) > 4 || (osrelease(1) == 4 && osrelease(2) >=5))
      % Kernel too old to provide proper PRIME sync:
      fprintf('Your Linux kernel %s is too old to support proper hybrid graphics with Intel graphics chips.\n', c.kern.osrelease);
      fprintf('However, upgrading an Ubuntu Linux flavor or derivative to a suitable kernel is trivial.\n');
      fprintf('Some small subset of hybrid graphics laptops would allow you to utilize the powerful gpu\n');
      fprintf('in a dual X-Screen configuration for the external video outputs nonetheless, although this\n');
      fprintf('assistant can not help you with setting them up.\n');
      fprintf('Check ''help HybridGraphics'' and the Psychtoolbox website for instructions, follow them, then retry.\n\n');
      return;
    end

    if ~primeuseable
      fprintf('Your version of X-Server or Mesa is too old to support proper hybrid graphics on most laptops\n');
      fprintf('with Intel graphics chips. Upgrading to a minimum of X-Server 1.16.3 or later, Mesa 10.5.2 is\n');
      fprintf('required, the more recent versions the better! Hybrid graphics was successfully tested with\n');
      fprintf('Ubuntu 14.04.5 LTS, with XServer 1.18.3, Mesa 11.2.0 and on later on Ubuntu 16.04.1 LTS, with\n');
      fprintf('Intel integrated graphics chip + both NVidia and AMD discrete graphics cards, so we strongly\n');
      fprintf('recommend to use one of these tested versions/configurations or later ones.\n');
      fprintf('Some small subset of hybrid graphics laptops would allow you to utilize the powerful gpu\n');
      fprintf('in a dual X-Screen configuration for the external video outputs nonetheless, although this\n');
      fprintf('assistant can not help you with setting them up.\n');
      fprintf('Check ''help HybridGraphics'' and the Psychtoolbox website for instructions, follow them, then retry.\n\n');
      return;
    end

    % DRI3/Present PRIME render offloading is possible on this setup.
    fprintf('This hardware + software setup should allow use of the discrete gpu for faster rendering.\n');
    fprintf('Enabling DRI3/Present support is needed for that, i will do that for you.\n');
    fprintf('You will also need to setup your system to use the powerful gpu with Matlab or Octave.\n');
    fprintf('Check ''help HybridGraphics'' and the Psychtoolbox website for instructions, how to do that.\n');
    fprintf('While with most modern hybrid graphics laptops it should then just work, some more exotic\n');
    fprintf('models may still not be able to output anything with good timing to external video outputs.\n');
    fprintf('I can not detect myself if your laptop is such an exotic machine or not, so you will have to try.\n');
    fprintf('Such models can then be set up in a dual-X-Screen configuration to make them work with\n');
    fprintf('research grade timing, but this assistant can not set them up automatically, so you would\n');
    fprintf('have to do it manually. See ''help HybridGraphics'' and the Psychtoolbox website for instructions.\n\n');
    suitable = 1;
    fullysupported = 1;
    return;
  end

  % Non Intel display gpu, ie., either NVidia + NVidia or AMD + AMD?
  if isempty(strfind(winfo.DisplayCoreId, 'NVidia')) && isempty(strfind(winfo.DisplayCoreId, 'AMD'))
    % Nope, something else like VC4 - No hybrid graphics.
    return;
  end

  % Dual NVidia or dual AMD. At this point in time it would require a specially
  % hacked custom kernel from Mario Kleiner + a few more configuration hacks.
  if ~primeuseable
    fprintf('Your version of X-Server or Mesa is too old to support proper hybrid graphics on most laptops\n');
    fprintf('with NVidia/AMD graphics chips. Upgrading to a minimum of X-Server 1.16.3 or later, Mesa 10.5.2 is\n');
    fprintf('required, the more recent versions the better!\n');
    fprintf('Some small subset of hybrid graphics laptops would allow you to utilize the powerful gpu\n');
    fprintf('in a dual X-Screen configuration for the external video outputs nonetheless, although this\n');
    fprintf('assistant can not help you with setting them up.\n');
    fprintf('Check ''help HybridGraphics'' and the Psychtoolbox website for instructions, follow them, then retry.\n\n');
    return;
  end

  % No hope for amdgpu kms/ddx driven cards atm.:
  if strcmp(xdriver, 'amdgpu')
    fprintf('Your dual AMD graphics card laptop does not support Enduro with proper visual timing at\n');
    fprintf('this point in time for most Enduro laptop models, sorry.\n');
    fprintf('Some small subset of Enduro laptops would allow you to utilize the powerful gpu\n');
    fprintf('in a dual X-Screen configuration for the external video outputs nonetheless, although this\n');
    fprintf('assistant can not help you with setting them up.\n');
    fprintf('Check ''help HybridGraphics'' and the Psychtoolbox website for background info.\n\n');
    return;
  end

  if strcmp(xdriver, 'ati')
    fprintf('Your dual-gpu AMD Enduro laptop currently requires a specially hacked Linux kernel\n');
    fprintf('and some additional dirty configuration hacks to make Enduro work.\n');
  else
    fprintf('Your dual-gpu NVidia Optimus laptop currently requires a specially hacked Linux kernel\n');
    fprintf('to make Optimus work.\n');
  end
  fprintf('I will perform some of the required configuration steps if you want, but some manual work is left for you.\n');
  fprintf('Check ''help HybridGraphics'' and the Psychtoolbox website for background info.\n\n');
  fprintf('Some small subset of hybrid graphics laptops would allow you to utilize the powerful gpu\n');
  fprintf('in a dual X-Screen configuration for the external video outputs without special kernels or hacks,\n');
  fprintf('although this assistant can not help you with setting them up.\n');
  fprintf('Check ''help HybridGraphics'' and the Psychtoolbox website for background info.\n\n');
  suitable = 1;
  return;
end
