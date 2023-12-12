function varargout = PsychVulkan(cmd, varargin)
% PsychVulkan - Interface with the Vulkan graphics and compute api for special purpose tasks.
%
% This function allows to utilize the Khronos Vulkan rendering and compute api
% for special purpose display tasks on suitable operating systems with suitable
% Vulkan v1.1+ capable graphics and display hardware.
%
% Most often you won't call this function directly, but Psychtoolbox will call
% it appropriately from the PsychImaging() function. Read relevant sections
% related to Vulkan in 'help PsychImaging' first, before venturing into the
% functions offered by this function!
%
% Commands and their meaning:
% ---------------------------
%
% oldVerbosity = PsychVulkan('Verbosity' [, verbosity]);
% - Returns and optionally sets level of 'verbosity' for driver debug output.
%   'verbosity' = New level of verbosity: 0 = Silent, 1 = Errors only, 2 = Warnings,
%   3 = Info, 4 = Debug, 5 -- ... = Higher debug levels.
%
%
% isSupported = PsychVulkan('Supported');
% - Returns if use of the Vulkan rendering and display api is in principle supported
%   on this setup. 1 = Supported, 0 = No driver, hardware or operating system support.
%
% oldFlags = PsychVulkan('OverrideFlags' [, flags]);
% - Returns and optionally sets override for 'flags' parameter in low-level
%   function PsychVulkan('PerformPostWindowOpenSetup' ..., flags ...); for
%   driver debugging and testing. By default, override flags are not set or
%   used.
%

% History:
% 28-Jun-2020   mk  Written.

global GL;
persistent verbosity;
persistent supported;
persistent vulkan;
persistent usedOutputs;
persistent outputMappings;
persistent noglfinish;
persistent ovrFlags;

% Fast path dispatch of functions called from within Screen() imaging pipeline
% processing slots. Numeric 'cmd' codes, placed here for most efficient execution:
if nargin > 0 && isscalar(cmd) && isnumeric(cmd)
    if cmd == 0
        % Execute flip operation via a Vulkan Present operation at the appropriately
        % scheduled requested visual stimulus onset time:
        win = varargin{1};
        vwin = varargin{2};
        tWhen = varargin{3};

        if varargin{4} == 0
            doTimestamp = 2; % High precision system-provided scheduling/timestamping if possible.
        else
            doTimestamp = 0;
        end

        % Does underlying Vulkan driver support high-precision native timestamping?
        if vulkan{win}.SupportsTiming
            % Yes. Present and get maximum precision and reliability timestamp from Vulkan driver:
            predictedOnset = PsychVulkanCore('Present', vwin, tWhen, doTimestamp);

            % Inject predictedOnset == visual stimulus onset time into Screen(), for usual handling
            % and reporting back to usercode via Screen('Flip'):
            Screen('Hookfunction', win, 'SetOneshotFlipResults', '', predictedOnset);
        else
            % No. Need to use fallback with the help of Screen():

            % Assign this windows usedOutput as rank 0 output setting in Screen:
            % This affects beamposition based Vblank timestamping and Beamposition
            % in Screen('GetWindowInfo'), so we query scanout position of the correct
            % display engine for this win'dow:
            outputIndex = vulkan{win}.usedOutput;
            screenId = vulkan{win}.screenId;
            if ~isempty(outputIndex)
                Screen('Preference', 'ScreenToHead', screenId, outputMappings{screenId + 1}(1, outputIndex + 1), outputMappings{screenId + 1}(2, outputIndex + 1), 0);
            end

            % Perform a blocking Vulkan Present operation of the rendered interop image
            % to the display of Vulkan window vwin associated with onscreen window win.
            %
            % vblTime is the visual stimulus onset time, as computed by PsychVulkanCore's
            % own timestamping. This would only be accurate if the underlying Vulkan driver
            % supported high-precision timestamping, which it doesn't if we are in this path.
            % Therefore it is a simple GetSecs() style approximation:
            vblTime = PsychVulkanCore('Present', vwin, tWhen, doTimestamp);

            % As long as we don't have high precision timestamp support in PsychVulkanCore,
            % use Screen()'s VBLANK timestamps as reasonably accurate and mostly reliable surrogate:
            winfo = Screen('GetWindowInfo', win, 7);

            % Restore rank 0 output setting in Screen:
            if ~isempty(outputIndex)
                Screen('Preference', 'ScreenToHead', screenId, outputMappings{screenId + 1}(1, 1), outputMappings{screenId + 1}(2, 1), 0);
            end

            % predictedOnset is the last known vbl timestamp from Screen():
            predictedOnset = winfo.LastVBLTime;

            % If predictedOnset is valid and not stale, use it. Otherwise fall back to vblTime:
            if (predictedOnset > 0) && (predictedOnset ~= vulkan{win}.LastVBLTime)
                vblTime = predictedOnset;
            else
                predictedOnset = vblTime;
            end

            vulkan{win}.LastVBLTime = predictedOnset;

            % Inject vblTime and visual stimulus onset time into Screen(), for usual handling
            % and reporting back to usercode via Screen('Flip'), also current beamposition:
            Screen('Hookfunction', win, 'SetOneshotFlipResults', '', vblTime, predictedOnset, [], winfo.Beamposition);
        end

        return;
    end
    
    if cmd == 1
        % Vulkan window close operation, closes the Vulkan onscreen window associated with
        % a PTB onscreen window. Called from Screen('Close', win) and Screen('CloseAll') as
        % well as from usual "close window on error" error handling pathes:
        win = varargin{1};
        screenId = vulkan{win}.screenId;

        PsychVulkanCore('CloseWindow', vulkan{win}.vwin);

        % Remove windows display output from set of used outputs for windows screenId:
        usedOutputs{screenId + 1} = setdiff(usedOutputs{screenId + 1}, vulkan{win}.usedOutput);

        % TODO: Restore gamma lut on individual outputs or screens?

        % Restore rank 0 output setting in Screen:
        if ~isempty(vulkan{win}.usedOutput)
            Screen('Preference', 'ScreenToHead', screenId, outputMappings{screenId + 1}(1, 1), outputMappings{screenId + 1}(2, 1), 0);
        end

        if vulkan{win}.needsNvidiaWa
            % Reenable output at auto-selected preferred mode and old (x,y) starting location of viewport in X-Screen space:
            syscmd = sprintf('sleep 1; xrandr --screen %i --output %s --auto --pos %ix%i ; sleep 1', screenId, vulkan{win}.outputName, ceil(vulkan{win}.windowRect(1)), ceil(vulkan{win}.windowRect(2)));
            system(syscmd);
        end

        % Do we need a complete driver shutdown to work around Mesa < 20.1.2 bugs?
        if vulkan{win}.needsMesaDDMWa
            PsychVulkanCore('Close');
            clear usedOutputs;
        end

        return;
    end

    % Special present code for macOS, to work around Apple's broken Metal
    % implementation and other macOS bugs:
    if cmd == 2
        % Execute flip operation via a Vulkan Present operation at the appropriately
        % scheduled requested visual stimulus onset time:
        win = varargin{1};
        vwin = varargin{2};
        tWhen = varargin{3};

        if varargin{4} == 0
            doTimestamp = 2; % High precision system-provided scheduling/timestamping if possible.
        else
            doTimestamp = 0;
        end

        % Present and maybe get an ok precision and not too unreliable timestamp from Vulkan driver:
        predictedOnset = PsychVulkanCore('Present', vwin, tWhen, doTimestamp);

        % Get a second opinion on onset time from Screen's VBLANK timestamping:
        winfo = Screen('GetWindowInfo', win, 7);

        % Likely valid vblTime returned from Screen()?
        if (winfo.VBLEndline > 0) && (winfo.LastVBLTime > 0)
            % Yes. Assign:
            vblTime = winfo.LastVBLTime;
        else
            % No. Fallback to GetSecs as a noisy last resort:
            vblTime = GetSecs;
        end

        % If predictedOnset is valid, use it. Otherwise fall back to vblTime:
        if predictedOnset > 0
            % Valid timestamp from Vulkan? Validate a bit and warn if not:
            if (verbosity > 4) || ((verbosity > 1) && (abs(predictedOnset - vblTime) > 0.001))
                fprintf('PsychVulkan-DEBUG: Delta between Vulkan and reference timestamps is %f usecs.\n', 1e6 * (predictedOnset - vblTime));
            end

            vblTime = predictedOnset;
        elseif predictedOnset == 0
            % Vulkan timestamping returned bogus timestamp zero, but no diagnosed failure.
            % Use what we got from Screen() or GetSecs():
            predictedOnset = vblTime;

            if verbosity > 1
                fprintf('PsychVulkan-DEBUG: Vulkan timestamping failed. Falling back to reference timestamp %f secs. Timing or visual stimulation might be broken.\n', vblTime);
            end
        else
            % Error code timestamp -1 returned. We can't recover in a
            % meaningful way from that, just pass it through...
            vblTime = -1;

            if verbosity > 1
                fprintf('PsychVulkan-DEBUG: Vulkan timestamping failed completely. Returning invalid timestamp -1.\n');
            end
        end

        % Inject vblTime and visual stimulus onset time into Screen(), for usual handling
        % and reporting back to usercode via Screen('Flip'), also current beamposition:
        Screen('Hookfunction', win, 'SetOneshotFlipResults', '', vblTime, predictedOnset, [], winfo.Beamposition);

        return;
    end % Of macOS special code.
end % Of fast-path dispatch.

% Slow path dispatch:
if nargin < 1 || isempty(cmd)
  help PsychVulkan;
  fprintf('\n\nAlso available are functions from PsychVulkanCore:\n');
  PsychVulkanCore;
  return;
end

if isempty(usedOutputs)
    % Init list of usedOutputs for fullscreen/direct-display mode to empty:
    usedOutputs = cell(length(Screen('Screens')), 1);

    % Store Screen()'s original screenId -> output/display engine mapping:
    outputMappings = cell(size(usedOutputs));
    for i = Screen('Screens')
        outputMappings{i + 1} = Screen('Preference','ScreenToHead', i);
    end

    try
        if exist('PsychVulkanCore', 'file')
            verbosity = PsychVulkanCore('Verbosity');
        else
            verbosity = 3;
        end
    catch
        lasterror('reset'); %#ok<*LERR>
        verbosity = 3;
    end
end

if strcmpi(cmd, 'Verbosity')
    varargout{1} = verbosity;

    if (~isempty(varargin)) && ~isempty(varargin{1})
        verbosity = varargin{1};

        if exist('PsychVulkanCore', 'file')
            try
                PsychVulkanCore('Verbosity', verbosity);
            catch
                lasterror('reset');
            end
        end
    end

    return;
end

if strcmpi(cmd, 'OverrideFlags')
    varargout{1} = ovrFlags;

    if ~isempty(varargin)
        ovrFlags = varargin{1};
    end

    return;
end

% isSupported = PsychVulkan('Supported');
if strcmpi(cmd, 'Supported')
    % Init supported flag via one-time probe:
    if isempty(supported)
        try
            if exist('PsychVulkanCore', 'file') && (PsychVulkanCore('GetCount') > 0) && ...
               (~IsOSX || IsMinimumOSXVersion(10,15,4)) % macOS 10.15.4 is the bare minimum needed.
                supported = 1;
            else
                supported = 0;
            end

            if isempty(verbosity)
                verbosity = 3;
                PsychVulkanCore('Verbosity', verbosity);
            end
        catch
            lasterror('reset');
            supported = 0;
        end
    end

    varargout{1} = supported;

    return;
end

% [winRect, ovrfbOverrideRect, ovrSpecialFlags, outputName] = PsychVulkan('OpenWindowSetup', outputName, screenId, winRect, ovrfbOverrideRect, ovrSpecialFlags);
if strcmpi(cmd, 'OpenWindowSetup')
    outputName = varargin{1};
    screenId = varargin{2};
    winRect = varargin{3};
    ovrfbOverrideRect = varargin{4};
    ovrSpecialFlags = varargin{5};
    if isempty(ovrSpecialFlags)
        ovrSpecialFlags = 0;
    end

    outputIndex = [];

    % On Linux X11 one can select a single video output via outputName parameter or winRect:
    if IsLinux && ~IsWayland
        if ~isempty(outputName)
            % Try to find the output with the requested name on requested X-Screen screenId:
            output = [];
            for i = 0:Screen('ConfigureDisplay', 'NumberOutputs', screenId)-1
                % Skip this output i if it is in the set of outputs used on this
                % screenId in direct display mode already, ie. RandR leased:
                if ismember(i, usedOutputs{screenId + 1})
                    continue;
                end

                output = Screen('ConfigureDisplay', 'Scanout', screenId, i);
                if strcmp(output.name, outputName)
                    % This output i is the right output.
                    % Position our onscreen window accordingly:
                    outputIndex = i;
                    winRect = OffsetRect([0, 0, output.width, output.height], output.xStart, output.yStart);

                    if verbosity >= 4
                        fprintf('PsychVulkan-INFO: Positioning onscreen window at rect [%i, %i, %i, %i] to align with display output %i [%s] on screen %i.\n', ...
                                winRect(1), winRect(2), winRect(3), winRect(4), i, outputName, screenId);
                    end

                    break;
                else
                    output = [];
                    outputIndex = [];
                end
            end

            if isempty(output)
                % No such output with outputName!
                sca;
                error('PsychVulkan-ERROR: Invalid outputName ''%s'' requested for Vulkan fullscreen display on screen %i. No such output available or output already in fullscreen use.', outputName, screenId);
            end
        else
            % No outputName given, 'winRect' provided?
            if ~isempty(winRect)
                % Yes. Does it match an attached RandR output exactly?
                output = [];
                for i = 0:Screen('ConfigureDisplay', 'NumberOutputs', screenId)-1
                    % Skip this output i if it is in the set of outputs used on this
                    % screenId in direct display mode already, ie. RandR leased:
                    if ismember(i, usedOutputs{screenId + 1})
                        continue;
                    end

                    output = Screen('ConfigureDisplay', 'Scanout', screenId, i);
                    outputRect = OffsetRect([0, 0, output.width, output.height], output.xStart, output.yStart);
                    if isequal(winRect, outputRect)
                        % This output i is the right output.
                        outputName = output.name;
                        outputIndex = i;
                        if verbosity >= 4
                            fprintf('PsychVulkan-INFO: Onscreen window at rect [%i, %i, %i, %i] is aligned with display output %i [%s] of screen %i.\n', ...
                                    winRect(1), winRect(2), winRect(3), winRect(4), i, outputName, screenId);
                        end

                        break;
                    else
                        output = [];
                        outputIndex = [];
                    end
                end

                % Does an output 'outputName' match the winRect?
                if isempty(output)
                    % No. So the non-empty winRect specifies a non-fullscreen window,
                    % only covering part of an X-Screen and part of outputs. Iow.,
                    % this is a windowed window:
                    outputName = [];
                end
            else
                % Empty winRect on a Linux X11 screen. Assume fullscreen on primary output for screenId:
                i = 0;

                % Skip this output i if it is in the set of outputs used on this
                % screenId in direct display mode already, ie. RandR leased:
                while ismember(i, usedOutputs{screenId + 1})
                    % Try next available output on the screenId:
                    i = i + 1;
                    if i == Screen('ConfigureDisplay', 'NumberOutputs', screenId)
                        % No more outputs available - All are already leased in
                        % direct display mode:
                        sca;
                        error('PsychVulkan-ERROR: Could not find a free output for Vulkan fullscreen display on screen %i. All outputs are already in fullscreen use.', screenId);
                    end
                end

                output = Screen('ConfigureDisplay', 'Scanout', screenId, i);
                outputName = output.name;
                outputIndex = i;

                % Update winRect accordingly:
                winRect = OffsetRect([0, 0, output.width, output.height], output.xStart, output.yStart);
                if verbosity >= 4
                    fprintf('PsychVulkan-INFO: Positioning onscreen window at rect [%i, %i, %i, %i] to align with display output %i [%s] of screen %i.\n', ...
                            winRect(1), winRect(2), winRect(3), winRect(4), i, outputName, screenId);
                end
            end
        end
    else
        % Not Linux X11: Linux DRM/KMS VT, Linux Wayland, MS-Windows etc.
        if isempty(winRect)
            % No winRect given: Means fullscreen on a specific monitor, defined by screenId:
            winRect = Screen('GlobalRect', screenId);
            outputName = 1;
            outputIndex = 0;
        else
            % Non-empty winRect: Fullscreen on monitor defined by screenId?
            if isequal(winRect, Screen('GlobalRect', screenId))
                % Yes: Fullscreen display:
                outputName = 1;
                outputIndex = 0;
            else
                % No: Windowed non-fullscreen window:
                outputName = [];
                outputIndex = [];
            end
        end

        if ~isempty(outputName)
            if verbosity >= 3
                fprintf('PsychVulkan-INFO: Onscreen window at rect [%i, %i, %i, %i] is aligned with fullscreen exclusive output for screenId %i.\n', ...
                        winRect(1), winRect(2), winRect(3), winRect(4), screenId);
            end
        end
    end

    % If a fullscreen output is assigned, then set its crtc and display engine
    % as rank 0 primary output:
    if ~isempty(outputIndex)
        Screen('Preference', 'ScreenToHead', screenId, outputMappings{screenId + 1}(1, outputIndex + 1), outputMappings{screenId + 1}(2, outputIndex + 1), 0);
    end

    if ~IsOSX
        % These always have to match:
        ovrfbOverrideRect = winRect;
    end

    % Set ovrSpecialFlags override settings to mark the onscreen window as not
    % important for visual stimulation, because the actual window / OpenGL windowing
    % backend is not used for primary stimulus display. Instead we / Vulkan/WSI is
    % in charge for proper visual stimulus display to the actual display monitor.
    % This will cause Screen() to skip certain tests or calibrations and omit certain
    % warnings, e.g., wrt. timing precision, active desktop compositors etc., as they
    % don't really apply - or at least not in a way Screen() can deal with, as it is
    % the Vulkan drivers job to handle that:
    ovrSpecialFlags = mor(ovrSpecialFlags, kPsychExternalDisplayMethod);

    % Assign modified return args:
    varargout{1} = winRect;
    varargout{2} = ovrfbOverrideRect;
    varargout{3} = ovrSpecialFlags;
    varargout{4} = outputName;

    return;
end

% vwin = PsychVulkan('PerformPostWindowOpenSetup', window, windowRect, isFullscreen, outputName, hdrMode, colorPrecision, colorSpace, colorFormat, gpuIndex, flags)
if strcmpi(cmd, 'PerformPostWindowOpenSetup')
    % Setup operations after Screen's PTB onscreen window is opened, and OpenGL and
    % the imaging pipeline are brought up. Needs to hook up the imaging pipeline to
    % ourselves and the PsychVulkanCore low-level driver.

    % noglfinish, if set to 1, will avoid glFinish() for OpenGL->Vulkan sync, but
    % instead request and use a shared semaphore for sync, which should be a bit
    % faster in theory:
    noglfinish = 1;

    % Must have global GL constants:
    if isempty(GL)
        varargout{1} = 0;
        warning('PTB internal error in PsychVulkan: GL struct not initialized?!?');
        return;
    end

    % Psychtoolbox Screen onscreen window handle:
    win = varargin{1};

    % Window position and size rectangle:
    windowRect = varargin{2};

    % Fullscreen flag: 1 = Take over a whole monitor, 0 = Operate as regular window.
    isFullscreen = varargin{3};

    % Display output name - Only relevant on Linux/X11 atm.:
    outputName = varargin{4};

    % hdrMode: 0 = SDR, 1 = HDR-10:
    hdrMode = varargin{5};

    % colorPrecision: 0 = 8 bpc RGBA8, 1 = 10 bpc RGB10_A2, 2 = fp16 RGBA16F half-float:
    colorPrecision = varargin{6};

    % VkColorSpace id. If empty, then will be set automatically according to hdrMode:
    colorSpace = varargin{7};

    % VkFormat color format. If empty, then will be set automatically according to colorPrecision and/or hdrMode:
    colorFormat = varargin{8};

    % gpuIndex of Vulkan driver+gpu combo to use: 0 = Auto-Select, 1 = 1st, 2 = 2nd, ... gpu.
    gpuIndex = varargin{9};

    % Optional flags, and'ed together: +1 = Diagnostic display only, no interop:
    flags = varargin{10};

    % Override flags specified?
    if ~isempty(ovrFlags)
        % Yes. Use them as override:
        fprintf('PsychVulkan-INFO: Global low level override flags %i specified via PsychVulkan(''OverrideFlags''). Using it instead of caller-provided flags %i.\n', ovrFlags, flags);
        flags = ovrFlags;
    end

    winfo = Screen('GetWindowInfo', win);
    screenId = Screen('WindowScreenNumber', win);
    refreshHz = Screen('Framerate', screenId);
    if refreshHz == 0
        % macOS reports refresh rate of 0 on Mac builtin panels - the idiocy...
        refreshHz = 60;
    end

    devs = PsychVulkanCore('GetDevices');

    % Restore rank 0 output setting in Screen:
    Screen('Preference', 'ScreenToHead', screenId, outputMappings{screenId + 1}(1, 1), outputMappings{screenId + 1}(2, 1), 0);

    % NVidia gpu under Linux/X11 with NVIDIA proprietary driver? And onscreen window fills complete target X-Screen?
    % Or this is a single-output display NVidia Optimus PRIME render offload setup, where NVIDIA's RandR output leasing does not work?
    if IsLinux && ~IsWayland && ~isempty(strfind(winfo.GLVendor, 'NVIDIA')) && ...
       (isequal(Screen('GlobalRect', win), Screen('GlobalRect', screenId)) || ~isempty(getenv('__NV_PRIME_RENDER_OFFLOAD')))
        % Do not use direct display mode via RandR output leasing. This window can
        % be pageflipped under X11 as well, without need for Vulkan direct display:
        isFullscreen = 0;
    end

    % AMD gpu under MS-Windows?
    if IsWin && ~isempty(strfind(winfo.GLVendor, 'ATI'))
        % For some of these the AMD Vulkan driver is buggy in that
        % switching to fullscreen-exclusive mode for fullscreen windows
        % causes massive malfunctions and a black screen display only,
        % e.g., the Radeon RX 460 (Polaris, pci device id 0x67EF).
        % Check gpu against badFSEIds list and enable a workaround if it is
        % one of the bad gpu's.
        % Additionally driver version raw >= 8388767 aka 20.11.2+ to
        % somewhere before 23.11.1 seems to have generally broken
        % fullscreen-exclusive mode, as verified by Dale Stolizka, and by
        % kleinerm with version 21.11.2 from one year later - November
        % 2021! This on the Windows 10 21H1 edition. So we fall back to
        % non-fs-exclusive mode and accept broken timing and potentially
        % impaired HDR - what choice do we have?! The bug seems to be gone
        % as of version 23.11.1, aka raw 8388887, so we can enable
        % fs-exclusive again for these recent driver from November 2023.
        badFSEIds = hex2dec({'67EF'});
        for i=1:length(devs)
            if (devs(i).VendorId == 4098) && strcmp(winfo.GLRenderer, devs(i).GpuName) && ...
               (ismember(devs(i).DeviceId, badFSEIds) || (devs(i).DriverVersionRaw >= 8388767 && devs(i).DriverVersionRaw < 8388887))
                % Got a bad one! Disable fullscreen-exclusive mode for fullscreen windows:
                flags = mor(flags, 2);
                fprintf('PsychVulkan-INFO: AMD gpu [%s] with buggy Vulkan driver for fullscreen mode detected! Enabling workaround, timing reliability may suffer.\n', devs(i).GpuName);
            end
        end
    end

    if isempty(strfind(glGetString(GL.EXTENSIONS), 'GL_EXT_memory_object')) || isempty(strfind(glGetString(GL.EXTENSIONS), 'GL_EXT_direct_state_access')) %#ok<STREMP>
        % If no specific Vulkan gpu was requested, select the first non-AMD/NVidia
        % gpuIndex on Linux or Windows: AMD and NVidia OpenGL fully support
        % OpenGL interop, so if we end here then the render-gpu can not be an
        % AMD or NVidia, ergo the display gpu should not be one.
        %
        % Also ignore MoltenVK (DriverId == 14) on macOS, because Apple's
        % OpenGL does not support this extension at all, so we can't use
        % this as selection criterion. Instead we just choose the first
        % enumerated gpu on macOS: On a single-gpu machine that is the
        % obviously correct choice. On a dual-gpu MacBookPro, the 1st gpu
        % seems to be the discrete high-performance gpu, which matches
        % Screen()'s choice of gpu as OpenGL renderer, so again gpuIndex 1
        % would give us a match.
        %
        % Default also to gpuIndex 1 in case this filtering fails to find an eligible gpu:
        if isempty(gpuIndex) || gpuIndex == 0
            gpuIndex = 1;
            for i=1:length(devs)
                if ~ismember(devs(i).DriverId, [1, 2, 3, 4, 14])
                    gpuIndex = devs(i).DeviceIndex;
                end
            end
        end

        if IsOSX
            noInterop = 0;

            % Disable Direct-To-Display mode, it is buggy as hell, at least
            % as tested on macOS 10.15.7 and 12.6 with AMD Radeon Pro 560.
            % Not that it works much better with this hack, it is only a
            % bit better. At the same time, this hack supposedly adds one
            % frame of extra latency, but our measurements show that even
            % without it, there is one frame of extra latency, contrary to
            % what the docs wrt. Direct-to-Display mode say.
            % On macOS 13.3.1, it is still broken, but different: The one
            % frame latency is gone, but now stimulus onset scheduling is
            % broken whenever a flip is more than 2 frames in the future!
            %
            % Broken stuff all around on the iToys operating system:
            flags = mor(flags, 2);
        else
            flags = mor(flags, 1);
            noInterop = 1;

            fprintf('PsychVulkan-INFO: OpenGL implementation does not support OpenGL-Vulkan interop! Enabling basic diagnostic mode on gpu %i.\n', gpuIndex);
        end
    else
        noInterop = 0;
    end

    usedOutput = [];
    oldbpc = 0;

    if IsLinux
        if isFullscreen
            if ~isempty(outputName)
                % Try to find the output with the requested name:
                output = [];
                for i = 0:Screen('ConfigureDisplay', 'NumberOutputs', screenId)-1
                    if ismember(i, usedOutputs{screenId + 1})
                        continue;
                    end

                    output = Screen('ConfigureDisplay', 'Scanout', screenId, i);
                    if strcmp(output.name, outputName)
                        % This output i is the right output.
                        usedOutput = i;
                        break;
                    else
                        output = [];
                    end
                end
            else
                % Choose primary output for screenId:
                if ~ismember(0, usedOutputs{screenId + 1})
                    usedOutput = 0;
                    output = Screen('ConfigureDisplay', 'Scanout', screenId, 0);
                else
                    output = [];
                end
            end

            if isempty(output)
                sca;
                error('Failed to open Vulkan window: Could not find suitable fullscreen output.');
            end

            % On Linux in fullscreen mode, outputHandle encodes the X11 RandR XID
            % of the RandR output which we want to take over for direct mode display:
            outputHandle = uint64(output.outputHandle);
            outputName = output.name;
            refreshHz = output.hz;

            % Position our onscreen window accordingly:
            windowRect = OffsetRect([0, 0, output.width, output.height], output.xStart, output.yStart);
            if verbosity >= 3
                fprintf('PsychVulkan-INFO: Positioning onscreen window at rect [%i, %i, %i, %i] to align with display output %i [%s] of screen %i.\n', ...
                        windowRect(1), windowRect(2), windowRect(3), windowRect(4), usedOutput, outputName, screenId);
            end

            % More than 8 bpc output precision desired?
            % Note that colorPrecision == 0 and hdrMode > 0 gets handled automatically
            % by the Vulkan driver (amdvlk), ie. setup for 10 bpc -> 8/10 is fine.
            if colorPrecision > 0
                % Need to call SetWindowBackendOverrides early before Vulkan openwindow,
                % as potential RandR 'max bpc' output precision setup will no longer work
                % once the X-Server is locked out by Vulkan.
                % Therefore set bpc to the maximum possible, given our current information.
                % Later on we will call SetWindowBackendOverrides again after window open,
                % when we actually know the true effective framebuffer output precision.
                %
                % Assign override color depth and refresh interval for display:
                if colorPrecision == 1
                    % RGB10_A2:
                    bpc = 10;
                elseif colorPrecision == 2 || colorPrecision == 6
                    % fp16 ~ 11:
                    bpc = 11;
                else
                    % 16 bpc fixed point:
                    bpc = 16;
                end
                Screen('HookFunction', win, 'SetWindowBackendOverrides', [], bpc * 3, 1 / refreshHz);
                oldbpc = bpc;
            end
        else
            % On Linux in windowed mode, outputHandle encodes the X11 window handle of
            % the PTB onscreen window, which we will use for the Vulkan display:
            outputHandle = uint64(winfo.SysWindowHandle);

            % TODO XXX: Should we calculate refreshHz per output or from FlipInterval instead?
        end
    else
        if IsOSX
            % On macOS we need the CAMetalLayer backing the onscreen window in
            % kPsychExternalDisplayMethod mode. It is stored in SysWindowInteropHandle:
            outputHandle = uint64(winfo.SysWindowInteropHandle);
        else
            % On Windows, outputHandle is meaningless atm.:
            outputHandle = uint64(winfo.SysWindowHandle);
        end

        if isFullscreen
            % Mark output 0 (the only possible output for a screenId on
            % non-Linux/X11) of screenId as used:
            if ~ismember(0, usedOutputs{screenId + 1})
                usedOutput = 0;
            else
                % Output already used!
                sca;
                error('Failed to open Vulkan window: Tried to open fullscreen-exclusive output on screenId %i, but that one is already in use.', screenId);
            end
        end
    end

    % Get the UUID of the Vulkan device that is compatible with our associated
    % OpenGL renderer/gpu. Compatible means: Can by used for OpenGL-Vulkan interop:
    if ~isempty(winfo.GLDeviceUUID)
        targetUUID = winfo.GLDeviceUUID;
    else
        % None provided, because the OpenGL implementation does not support
        % OpenGL-Vulkan interop. Assign empty id for most basic testing:
        targetUUID = zeros(1, 16, 'uint8');
    end

    % Is the special fullscreen direct display mode workaround for NVidia blobs on Linux needed?
    needsNvidiaWa = IsLinux && isFullscreen && strcmp(winfo.DisplayCoreId, 'NVidia') && (~isempty(strfind(winfo.GLVendor, 'NVIDIA')) || noInterop);

    % Try to open the Vulkan window and setup Vulkan side of interop:
    try
        % Awful hack to deal with NVidia blobs limitations wrt. output leasing. Output leasing only works for disabled
        % outputs, so we have to shut the output down before opening a Vulkan window:
        if needsNvidiaWa
            system(sprintf('xrandr --screen %i --output %s --off ; sleep 1', screenId, outputName));
        end

        if hdrMode
            % We want an identity hardware gamma lut in HDR, but at maximum lut precision,
            % so output does not get truncated to 8 bpc. Therefore we can't use LoadIdentityClut()
            % which is aimed at 8 bpc identity pixel passthrough.

            % On Linux X11 we may have to address individual RandR outputs:
            if IsLinux && ~IsWayland
                physicalDisplay = usedOutput;
            else
                physicalDisplay = [];
            end

            % Backup old lut for this screen:
            BackupCluts(screenId);

            % Upload a perfectly linear lut for the given gpu:
            [~, ~, reallutsize] = Screen('ReadNormalizedGammaTable', win, physicalDisplay);

            % AMD gpu under Linux?
            if IsLinux && strcmp(winfo.DisplayCoreId, 'AMD')
                % Use special identity gamma table (like in LoadIdentityClut()) that
                % is known and verified to get recognized by amdgpu-kms DC and trigger
                % gamma table hardware bypass mode in hardware:
                identityLUT = (linspace(0, 1, 256)' * ones(1, 3));
            else
                % Other gpu + driver + os combo: Standard identity lut:
                identityLUT = repmat(linspace(0, 1, reallutsize)', 1, 3);
            end

            Screen('LoadNormalizedGammaTable', win, identityLUT, 0, physicalDisplay);

            if verbosity >= 3
                fprintf('PsychVulkan-INFO: Loaded identity gamma table into output for HDR.\n');
            end

            % HDR mode 1 aka HDR-10 on MS-Windows, in fullscreen, but fullscreen
            % exclusive mode disabled by flags?
            if IsWin && isFullscreen && (hdrMode == 1) && bitand(flags, 2) && (colorPrecision < 2)
                % Yes. The only reliably supported HDR-10 mode across gpu
                % vendors is fp16 scRGB. Enforce colorPrecision 2 == fp16,
                % which will enforce fp16 scRGB HDR under Windows DWM:
                colorPrecision = 2;
            end
        end

        % Open the Vulkan window:
        vwin = PsychVulkanCore('OpenWindow', gpuIndex, targetUUID, isFullscreen, screenId, windowRect, outputHandle, hdrMode, colorPrecision, refreshHz, colorSpace, colorFormat, flags);

        % No interop, or semaphores unsupported?
        if noInterop || isempty(strfind(glGetString(GL.EXTENSIONS), 'GL_EXT_semaphore')) %#ok<STREMP>
            if ~noInterop
                fprintf('PsychVulkan-INFO: OpenGL implementation does not support OpenGL-Vulkan interop semaphores. Enabling operation without semaphores on gpu %i.\n', gpuIndex);
            else
                fprintf('PsychVulkan-INFO: Interop disabled! Enabling operation without semaphores on gpu %i.\n', gpuIndex);
            end

            % In no-interop debug mode we  must not use semaphores, because they
            % are likely unsupported by the OpenGL or Vulkan driver as well, so
            % use classic fallback path with glFinish:
            noglfinish = 0;
        end

        % Get all required info for OpenGL-Vulkan interop:
        [interopObjectHandle, allocationSize, formatSpec, tilingMode, memoryOffset, width, height, renderCompleteSemaphore] = PsychVulkanCore('GetInteropHandle', vwin, noglfinish);
    catch
        % Failed! Reenable RandR output if this was a failed attempt at output leasing on Linux + NVidia:
        if needsNvidiaWa
            system(sprintf('sleep 1; xrandr --screen %i --output %s --auto --pos %ix%i ; sleep 1', screenId, outputName, ceil(windowRect(1)), ceil(windowRect(2))));
        end

        % Close all windows:
        sca;
        error('Failed to open Vulkan window.');
    end

    % We got the open Vulkan window, and the interop info. Setup OpenGL interop:

    % Selection of format for the OpenGL interop texture, matching what Vulkan selected:
    switch formatSpec
        case 0
            internalFormat = GL.RGBA8;
            bpc = 8;
            if verbosity >= 3
                fprintf('PsychVulkan-INFO: 8 bpc linear precision framebuffer will be used.\n');
            end

        case 1
            internalFormat = GL.RGB10_A2;
            bpc = 10;
            if verbosity >= 3
                fprintf('PsychVulkan-INFO: 10 bpc linear precision framebuffer will be used.\n');
            end

        case 2
            internalFormat = GL.RGBA16F;
            bpc = 11;
            if verbosity >= 3
                fprintf('PsychVulkan-INFO: 16 bpc non-linear half-float precision framebuffer will be used.\n');
            end

        case 3
            internalFormat = GL.RGBA16;
            bpc = 16;
            if verbosity >= 3
                fprintf('PsychVulkan-INFO: 16 bpc linear precision framebuffer will be used.\n');
            end

        otherwise
            sca;
            error('Unknown formatSpec provided!');
    end

    % Selection of OpenGL tiling mode for rendering into interop texture:
    if tilingMode
        tilingMode = GL.OPTIMAL_TILING_EXT;
        if verbosity >= 4
            fprintf('PsychVulkan-INFO: Using tiled rendering layout framebuffer for interop rendering.\n');
        end
    else
        tilingMode = GL.LINEAR_TILING_EXT;
        if verbosity >= 4
            fprintf('PsychVulkan-INFO: Using linear rendering layout framebuffer for interop rendering.\n');
        end
    end

    % Set it up:
    if ~noInterop
        if IsOSX
            oldtex = Screen('Hookfunction', win, 'SetDisplayBufferTextures', [], double(interopObjectHandle), [], GL.TEXTURE_RECTANGLE, internalFormat, 0, width, height);
            glDeleteTextures(1, oldtex); % Get rid of now unused old texture.
        else
            Screen('Hookfunction', win, 'ImportDisplayBufferInteropMemory', [], 0, interopObjectHandle, allocationSize, internalFormat, tilingMode, memoryOffset, width, height, renderCompleteSemaphore);
        end
    end

    vulkan{win}.valid = 1;
    vulkan{win}.win = win;
    vulkan{win}.vwin = vwin;
    vulkan{win}.width = width;
    vulkan{win}.height = height;
    vulkan{win}.isFullscreen = isFullscreen;
    vulkan{win}.screenId = screenId;
    vulkan{win}.windowRect = windowRect;
    vulkan{win}.outputHandle = outputHandle;
    vulkan{win}.outputName = outputName;
    vulkan{win}.needsNvidiaWa = needsNvidiaWa;
    vulkan{win}.LastVBLTime = nan;

    % Find out which Vulkan device was chosen to drive this window:
    hdrInfo = PsychVulkanCore('GetHDRProperties', vwin);
    gpuIndex = hdrInfo.GPUIndex;

    % Mesa open-source Vulkan drivers of Mesa version < 22.2.4 in use for fullscreen direct display mode and
    % target video output windowRect doesn't have top-left corner at (0,0)?
    if isFullscreen && any(windowRect(1:2)) && ismember(devs(gpuIndex).DriverId, [3, 6, 13, 18:20, 22:23]) && ...
       (devs(gpuIndex).DriverVersionRaw < bitshift (22, 22) + bitshift (2, 12) + 4)
        % Yes. At least as of Mesa version 22.3-devel and earlier (tested Mesa 22.3-git, 22.0.5, 21.2.6),
        % these drivers have an internal caching bug in their WSI, where in direct display mode they will
        % fail on any run after the first run in a session (vkQueuePresent() fails with VK_ERROR_SURFACE_LOST),
        % unless either the fullscreen output window was covering a video output with viewport (x,y) starting
        % at X-Screen position (0,0), ie. viewport top-left == X-Screen top-left. This is due to some erroneous
        % caching of internal per DRM/KMS output (wsi_display_connector->active) state, retaining stale settings
        % from the first session, which leads to omitting a required drmModeSetCrtc() full modeset on first
        % present -> Boom!
        %
        % The bug has been diagnosed and fixed by myself and will be part of Mesa 22.2.4 and later, but won't
        % get backported further, so we need a workaround for at least Debian 11, Ubuntu 20.04-LTS
        % and Ubuntu 22.04.1-LTS, Ubuntu 22.10 and possibly also Ubuntu 22.04.2-LTS.
        %
        % The workaround is a full driver shutdown -> VKInstance destruction -> Stale cache reset -> Ok:
        vulkan{win}.needsMesaDDMWa = 1;
        fprintf('PsychVulkan-WARNING: Need to enable full-driver-shutdown workaround for buggy Mesa Vulkan driver!\n');
        fprintf('PsychVulkan-WARNING: This could cause problems on multi-window Vulkan configurations and is a bad hack!\n');
        fprintf('PsychVulkan-WARNING: Windows placed in the origin (0,0) of their respective X-Screen are not affected.\n');
        fprintf('PsychVulkan-WARNING: Please upgrade to Mesa version 22.2.4 or later to get rid of this hack.\n');
    else
        vulkan{win}.needsMesaDDMWa = 0;
    end

    % Store for win'dow if Vulkan driver supports high-precision timing and timestamping natively:
    vulkan{win}.SupportsTiming = devs(gpuIndex).SupportsTiming;

    % Interop enabled. Set up callbacks from Screen() imaging pipeline into our driver:

    % Optimized method: Use semaphore for render completion signalling by OpenGL,
    % Do not reset "one-shot" flags, so we do not set them again before each flip.
    % We do all swap scheduling and timestamping, so Screen can skip OpenGL buffer swaps,
    % waits/scheduling/timestamping:
    Screen('Hookfunction', win, 'SetOneshotFlipFlags', '', kPsychDontAutoResetOneshotFlags + kPsychSkipWaitForFlipOnce + kPsychSkipSwapForFlipOnce + kPsychSkipTimestampingForFlipOnce);

    if ~noglfinish
        % Old method as fallback: Use glFinish to sync Vulkan with OpenGL, except on macOS where glFlush seems enough:
        if IsOSX
            Screen('Hookfunction', win, 'AppendMFunction', 'LeftFinalizerBlitChain', 'Vulkan Mono commit operation', 'moglcore(''glFlush'');');
        else
            Screen('Hookfunction', win, 'AppendMFunction', 'LeftFinalizerBlitChain', 'Vulkan Mono commit operation', 'moglcore(''glFinish'');');
        end
        Screen('Hookfunction', win, 'Enable', 'LeftFinalizerBlitChain');
    end

    if IsOSX
        % Special present path for Apples broken macOS:
        if ~vulkan{win}.SupportsTiming
            % We are screwed on macOS:
            sca;
            error('PsychVulkan-ERROR: macOS Vulkan does not provide builtin timing support. Game over!');
        end

        cmdString = sprintf('PsychVulkan(2, %i, %i, IMAGINGPIPE_FLIPTWHEN, IMAGINGPIPE_FLIPVBLSYNCLEVEL);', win, vwin);
    else
        % Well working operating systems:
        cmdString = sprintf('PsychVulkan(0, %i, %i, IMAGINGPIPE_FLIPTWHEN, IMAGINGPIPE_FLIPVBLSYNCLEVEL);', win, vwin);
    end
    Screen('Hookfunction', win, 'AppendMFunction', 'PreSwapbuffersOperations', 'Vulkan Present operation', cmdString);
    Screen('Hookfunction', win, 'Enable', 'PreSwapbuffersOperations');

    cmdString = sprintf('PsychVulkan(1, %i);', win);
    Screen('Hookfunction', win, 'PrependMFunction', 'CloseOnscreenWindowPreGLShutdown', 'Vulkan cleanup', cmdString);
    Screen('Hookfunction', win, 'Enable', 'CloseOnscreenWindowPreGLShutdown');

    % Avoid redundant call, if already done in Linux fullscreen path and effective
    % bpc has not changed since then. Redundant calls are not harmful, but produce
    % needless redundant status message clutter.
    if oldbpc ~= bpc
        % Assign override color depth and refresh interval for display:
        Screen('HookFunction', win, 'SetWindowBackendOverrides', [], bpc * 3, 1 / refreshHz);
    end

    % Mark usedOutput as being used by this window:
    vulkan{win}.usedOutput = usedOutput;

    % Mark usedOutput as being used on windows screenId:
    usedOutputs{screenId + 1} = union(usedOutputs{screenId + 1}, usedOutput);

    varargout{1} = vwin;

    return;
end

% 'cmd' so far not dispatched? Let's assume it is a command
% meant for PsychVulkanCore:
if (~isempty(varargin)) && ~isempty(varargin{1}) && isscalar(varargin{1}) && isreal(varargin{1}) && (Screen('WindowKind', varargin{1}) == 1)
  win = varargin{1};
  vwin = vulkan{win}.vwin;
  [ varargout{1:nargout} ] = PsychVulkanCore(cmd, vwin, varargin{2:end});
else
  [ varargout{1:nargout} ] = PsychVulkanCore(cmd, varargin{:});
end

end
