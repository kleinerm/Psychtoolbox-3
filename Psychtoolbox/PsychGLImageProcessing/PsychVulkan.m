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
% TODO XXX

% History:
% 28-Jul-2020   mk  Written.

global GL;
persistent verbosity;
persistent vulkan;
persistent usedOutputs;
persistent outputMappings;

% Fast path dispatch of functions called from within Screen() imaging pipeline
% processing slots. Numeric 'cmd' codes, placed here for most efficient execution:
if nargin > 0 && isscalar(cmd) && isnumeric(cmd)
    if cmd == 0
        % Preflip operations: After render completion, before flip.

        % Tell Screen() to skip regular flip scheduling and timestamping:
        win = varargin{1};
        Screen('Hookfunction', win, 'SetOneshotFlipFlags', '', kPsychSkipWaitForFlipOnce + kPsychSkipSwapForFlipOnce + kPsychSkipTimestampingForFlipOnce);

        % glFinish() the OpenGL pipeline. TODO: Switch to use of OpenGL<->Vulkan semaphores instead
        % for theoretically higher efficiency and correctness. In practice, this works on both
        % Linux and Windows-10 with AMD and NVidia, both OSS and proprietary drivers:
        glFinish;

        return;
    end
    
    if cmd == 1
        % Execute flip operation via a Vulkan Present operation at the appropriately
        % scheduled requested visual stimulus onset time:
        win = varargin{1};
        vwin = varargin{2};
        tWhen = varargin{3};

        if varargin{4} == 0
            doTimestamp = 1;
        else
            doTimestamp = 0;
        end

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
        % own timestamping. This is only accurate if the underlying Vulkan driver
        % supports high-precision timestamping. Otherwise it is a simple GetSecs()
        % style approximation: TODO: Implement actual high-precision support in our driver.
        vblTime = PsychVulkanCore('Present', vwin, tWhen, doTimestamp);

        % As long as we don't have high precision timestamp support in PsychVulkanCore,
        % use Screen()'s VBLANK timestamps as reasonably accurate and mostly reliable surrogate:
        winfo = Screen('GetWindowInfo', win, 7);

        % Restore rank 0 output setting in Screen:
        if ~isempty(outputIndex)
            Screen('Preference', 'ScreenToHead', screenId, outputMappings{screenId + 1}(1, 1), outputMappings{screenId + 1}(2, 1), 0);
        end

        predictedOnset = winfo.LastVBLTime;
        vblTime = predictedOnset;

        % Inject vblTime and visual stimulus onset time into Screen(), for usual handling
        % and reporting back to usercode via Screen('Flip'), also current beamposition:
        Screen('Hookfunction', win, 'SetOneshotFlipResults', '', vblTime, predictedOnset, [], winfo.Beamposition);

        return;
    end
    
    if cmd == 2
        % Vulkan window close operation, closes the Vulkan onscreen window associated with
        % a PTB onscreen window. Called from Screen('Close', win) and Screen('CloseAll') as
        % well as from usual "close window on error" error handling pathes:
        win = varargin{1};
        screenId = vulkan{win}.screenId;

        PsychVulkanCore('CloseWindow', vulkan{win}.vwin);

        % Remove windows display output from set of used outputs for windows screenId:
        usedOutputs{screenId + 1} = setdiff(usedOutputs{screenId + 1}, vulkan{win}.usedOutput);

        % Restore rank 0 output setting in Screen:
        if ~isempty(vulkan{win}.usedOutput)
            Screen('Preference', 'ScreenToHead', screenId, outputMappings{screenId + 1}(1, 1), outputMappings{screenId + 1}(2, 1), 0);
        end

        if vulkan{win}.needsNvidiaWa
            system(sprintf('xrandr --screen %i --output %s --auto ; sleep 1', screenId, vulkan{win}.outputName));
        end

        % Do we need a complete driver shutdown to work around Mesa < 20.1.2 bugs?
        if vulkan{win}.needsMesaDDMWa
            PsychVulkanCore('Close');
            clear usedOutputs;
        end

        return;
    end
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
        lasterror('reset');
        verbosity = 3;
    end
end

if strcmpi(cmd, 'Verbosity')
    varargout{1} = verbosity;

    if (length(varargin) > 0) && ~isempty(varargin{1})
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

if strcmpi(cmd, 'Supported')
    try
        if exist('PsychVulkanCore', 'file') && PsychVulkanCore('GetCount') >= 0
            varargout{1} = 1;
        else
            varargout{1} = 0;
        end

        if isempty(verbosity)
            verbosity = 3;
            PsychVulkanCore('Verbosity', verbosity);
        end
    catch
        lasterror('reset');
        varargout{1} = 0;
    end

    return;
end

% [winRect, ovrfbOverrideRect, ovrSpecialFlags, outputName] = PsychVulkan('OpenWindowSetup', outputName, screenId, winRect, ovrfbOverrideRect, ovrSpecialFlags);
if strcmpi(cmd, 'OpenWindowSetup')
    outputName = varargin{1};
    screenId = varargin{2};
    winRect = varargin{3};
    ovrfbOverrideRect = varargin{4}; %#ok<NASGU>
    ovrSpecialFlags = varargin{5};
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
                    fprintf('PsychVulkan-Info: Positioning onscreen window at rect [%i, %i, %i, %i] to align with display output %i [%s].\n', ...
                            winRect(1), winRect(2), winRect(3), winRect(4), i, outputName);
                    break;
                else
                    output = [];
                    outputIndex = [];
                end
            end

            if isempty(output)
                % No such output with outputName!
                sca;
                error('PsychVulkan-Error: Invalid outputName ''%s'' requested for Vulkan fullscreen display. No such output available or output already in fullscreen use.', outputName);
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

                        fprintf('PsychVulkan-Info: Onscreen window at rect [%i, %i, %i, %i] is aligned with display output %i [%s].\n', ...
                                winRect(1), winRect(2), winRect(3), winRect(4), i, outputName);
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
                        error('PsychVulkan-Error: Could not find a free output for Vulkan fullscreen display on screen %i. All outputs are already in fullscreen use.', screenId);
                    end
                end

                output = Screen('ConfigureDisplay', 'Scanout', screenId, i);
                outputName = output.name;
                outputIndex = i;

                % Update winRect accordingly:
                winRect = OffsetRect([0, 0, output.width, output.height], output.xStart, output.yStart);
                fprintf('PsychVulkan-Info: Positioning onscreen window at rect [%i, %i, %i, %i] to align with display output %i [%s].\n', ...
                        winRect(1), winRect(2), winRect(3), winRect(4), i, outputName);
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
            fprintf('PsychVulkan-Info: Onscreen window at rect [%i, %i, %i, %i] is aligned with fullscreen exclusive output for screenId %i.\n', ...
                    winRect(1), winRect(2), winRect(3), winRect(4), screenId);
        end
    end

    % If a fullscreen output is assigned, then set its crtc and display engine
    % as rank 0 primary output:
    if ~isempty(outputIndex)
        Screen('Preference', 'ScreenToHead', screenId, outputMappings{screenId + 1}(1, outputIndex + 1), outputMappings{screenId + 1}(2, outputIndex + 1), 0);
    end

    % These always have to match:
    ovrfbOverrideRect = winRect;

    % TODO XXX Define ovrSpecialFlags override settings?

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

    winfo = Screen('GetWindowInfo', win);
    screenId = Screen('WindowScreenNumber', win);
    refreshHz = Screen('Framerate', screenId);
    devs = PsychVulkanCore('GetDevices');

    % Restore rank 0 output setting in Screen:
    Screen('Preference', 'ScreenToHead', screenId, outputMappings{screenId + 1}(1, 1), outputMappings{screenId + 1}(2, 1), 0);

    if isempty(strfind(glGetString(GL.EXTENSIONS), 'GL_EXT_memory_object')) %#ok<STREMP>
        flags = mor(flags, 1);
        noInterop = 1;
        % If no specific Vulkan gpu was requested, select the first non-AMD/NVidia
        % gpuIndex (as we know AMD/NVidia can't be it - they fully support interop).
        % Default to gpuIndex 1 in case this filtering fails to find an elegible gpu:
        if isempty(gpuIndex) || gpuIndex == 0
            gpuIndex = 1;
            for i=1:length(devs)
                if ~ismember(devs(i).DriverId, [1, 2, 3, 4])
                    gpuIndex = devs(i).DeviceIndex;
                end
            end
        end
        fprintf('PsychVulkan-Info: OpenGL implementation does not support OpenGL-Vulkan interop! Enabling basic diagnostic mode on gpu %i.\n', gpuIndex);
    else
        noInterop = 0;
    end

    usedOutput = [];

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

                        % Position our onscreen window accordingly:
                        winRect = OffsetRect([0, 0, output.width, output.height], output.xStart, output.yStart);
                        fprintf('PsychVulkan-Info: Positioning onscreen window at rect [%i, %i, %i, %i] to align with display output %i [%s].\n', ...
                                winRect(1), winRect(2), winRect(3), winRect(4), i, outputName);
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
        else
            % On Linux in windowed mode, outputHandle encodes the X11 window handle of
            % the PTB onscreen window, which we will use for the Vulkan display:
            outputHandle = uint64(winfo.SysWindowHandle);

            % TODO XXX: Should we calculate refreshHz per output or from FlipInterval instead?
        end
    else
        % On Windows, outputHandle is meaningless atm.:
        outputHandle = uint64(0);
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
    needsNvidiaWa = IsLinux && isFullscreen && strcmp(winfo.DisplayCoreId, 'NVidia') && ~isempty(strfind(winfo.GLVendor, 'NVIDIA'));

    % Try to open the Vulkan window and setup Vulkan side of interop:
    try
        % Awful hack to deal with NVidia blobs limitations wrt. output leasing. Output leasing only works for disabled
        % outputs, so we have to shut the output down before opening a Vulkan window:
        if needsNvidiaWa
            system(sprintf('xrandr --screen %i --output %s --off ; sleep 1', screenId, outputName));
        end

        % Open the Vulkan window:
        vwin = PsychVulkanCore('OpenWindow', gpuIndex, targetUUID, isFullscreen, screenId, windowRect, outputHandle, hdrMode, colorPrecision, refreshHz, colorSpace, colorFormat, flags);

        % Get all required info for OpenGL-Vulkan interop:
        [interopObjectHandle, allocationSize, formatSpec, tilingMode, memoryOffset, width, height] = PsychVulkanCore('GetInteropHandle', vwin)
    catch
        % Failed! Reenable RandR output if this was a failed attempt at output leasing on Linux + NVidia:
        if needsNvidiaWa
            system(sprintf('xrandr --screen %i --output %s --auto ; sleep 1', screenId, outputName));
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
        case 1
            internalFormat = GL.RGB10_A2;
        case 2
            internalFormat = GL.RGBA16F;
        case 3
            internalFormat = GL.RGBA16;
        otherwise
            sca;
            error('Unknown formatSpec provided!');
    end

    % Selection of OpenGL tiling mode for rendering into interop texture:
    GL.OPTIMAL_TILING_EXT = hex2dec('9584');
    GL.LINEAR_TILING_EXT = hex2dec('9585');

    if tilingMode
        tilingMode = GL.OPTIMAL_TILING_EXT;
    else
        tilingMode = GL.LINEAR_TILING_EXT;
    end

    % Set it up:
    if ~noInterop
        Screen('Hookfunction', win, 'ImportDisplayBufferInteropMemory', [], 0, interopObjectHandle, allocationSize, internalFormat, tilingMode, memoryOffset, width, height);
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

    % Find out which Vulkan device was chosen to drive this window:
    hdrInfo = PsychVulkanCore('GetHDRProperties', vwin);
    gpuIndex = hdrInfo.GPUIndex;

    % Mesa opens-source AMD radv or Intel anvil driver of Mesa version < 20.1.2 in use for fullscreen direct display mode?
    if isFullscreen && ismember(devs(gpuIndex).DriverId, [3, 6]) && (devs(gpuIndex).DriverVersionRaw < bitshift (20, 22) + bitshift (1, 12) + 2)
        % These pre-20.1.2 drivers have a bug where in direct display mode they won't release the
        % display unless the complete Vulkan instance is destroyed. Iow. we need a full driver
        % shutdown:
        vulkan{win}.needsMesaDDMWa = 1;
        fprintf('PsychVulkan-WARNING: Need to enable full-driver-shutdown workaround for buggy Mesa Vulkan driver!\n');
        fprintf('PsychVulkan-WARNING: This may fail badly on multi-window configurations and is a bad hack!\n');
        fprintf('PsychVulkan-WARNING: Please upgrade to Mesa version 20.1.2 or later to get rid of this hack.\n');
    else
        vulkan{win}.needsMesaDDMWa = 0;
    end

    % Interop enabled. Set up callbacks from Screen() imaging pipeline into our driver:
    cmdString = sprintf('PsychVulkan(0, %i);', win);
    Screen('Hookfunction', win, 'AppendMFunction', 'LeftFinalizerBlitChain', 'Vulkan Mono commit operation', cmdString);
    Screen('Hookfunction', win, 'Enable', 'LeftFinalizerBlitChain');

    cmdString = sprintf('PsychVulkan(1, %i, %i, IMAGINGPIPE_FLIPTWHEN, IMAGINGPIPE_FLIPVBLSYNCLEVEL);', win, vwin);
    Screen('Hookfunction', win, 'AppendMFunction', 'PreSwapbuffersOperations', 'Vulkan Present operation', cmdString);
    Screen('Hookfunction', win, 'Enable', 'PreSwapbuffersOperations');

    cmdString = sprintf('PsychVulkan(2, %i);', win);
    Screen('Hookfunction', win, 'PrependMFunction', 'CloseOnscreenWindowPreGLShutdown', 'Vulkan cleanup', cmdString);
    Screen('Hookfunction', win, 'Enable', 'CloseOnscreenWindowPreGLShutdown');

    % Assign override color depth and refresh interval for display:
    Screen('HookFunction', win, 'SetWindowBackendOverrides', [], 24, 1 / refreshHz);

    % Mark usedOutput as being used by this window:
    vulkan{win}.usedOutput = usedOutput;

    % Mark usedOutput as being used on windows screenId:
    usedOutputs{screenId + 1} = union(usedOutputs{screenId + 1}, usedOutput);

    varargout{1} = vwin;

    return;
end

sca;
error('Invalid command ''%s'' specified. Read ''help PsychVulkan'' for list of valid commands.', cmd);

end
