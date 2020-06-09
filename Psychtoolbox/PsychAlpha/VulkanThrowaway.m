function VulkanThrowaway(cmd, varargin)
% VulkanThrowaway - Throwaway test code for Psychtoolbox OpenGL -> Vulkan integration.
%
global GL;

if nargin > 0 && isscalar(cmd) && isnumeric(cmd)
  if cmd == 0
    win = varargin{1};
    Screen('Hookfunction', win, 'SetOneshotFlipFlags', '', kPsychSkipWaitForFlipOnce + kPsychSkipSwapForFlipOnce + kPsychSkipTimestampingForFlipOnce);
    glFlush;
    return;
  end

  if cmd == 1
    win = varargin{1};
    vwin = varargin{2};
    tWhen = varargin{3};
    if varargin{4} == 0
      doTimestamp = 1;
    else
      doTimestamp = 0;
    end

    vblTime = PsychVulkanCore('Present', vwin, tWhen, doTimestamp);
    winfo = Screen('GetWindowInfo', win, 7);
    predictedOnset = winfo.LastVBLTime;
    Screen('Hookfunction', win, 'SetOneshotFlipResults', '', vblTime, predictedOnset);
    return;
  end

  if cmd == 2
    vwin = varargin{1};
    PsychVulkanCore('CloseWindow', vwin);
    %PsychVulkanCore('Close');
    return;
  end
end

colorSpace = 0;
colorFormat = 0;
flags = 0;

gpuIndex = 0
screenId = 0
isFullscreen = 1
colorPrecision = 0
hdrMode = 0

% For NVidia on screen 1 on darlene:
if screenId == 1
  PsychTweak ('UseGPUIndex', 0);
  Screen('Preference','ScreenTohead', screenId, 0, 0)
  if ~isFullscreen
    Screen('Preference', 'WindowShieldingLevel', -1);
  else
    Screen('Preference', 'WindowShieldingLevel', 2000);
  end
else
  Screen('Preference', 'WindowShieldingLevel', 2000);
end

PsychDefaultSetup(2);
%Screen('Preference', 'Verbosity', 10);
Screen('Preference', 'VBLTimestampingMode', 0);
Screen('Preference', 'ConserveVRAM', 524288);

InitializeMatlabOpenGL([],[],1);
GL.OPTIMAL_TILING_EXT = hex2dec('9584');
GL.LINEAR_TILING_EXT = hex2dec('9585');

multisample = 0;
stereomode = 0;
imagingmode = kPsychNeedFinalizedFBOSinks; % Useless, because we need at least a vertical flip OpenGL -> Vulkan, so no zero-copy possible anyway: + kPsychSinkIsMSAACapable;

if IsLinux
  if isFullscreen
    if screenId ~= 1 || 1
      output = Screen('ConfigureDisplay', 'Scanout', screenId, 0)
    else
      output.outputHandle = 551
      output.name = 'HDMI-0';
    end

    outputHandle = uint64(output.outputHandle);
    outputName = output.name
    if screenId == 1
      % Only for NVidia on screen 1: Need to disable output, otherwise RandR
      % leasing and direct display mode won't work with the NVidia blob :/
      % Screen('ConfigureDisplay', 'Scanout', screenId, 0, 0, 0, 0);
    end
  end
else
  sca;
  error('Fixup outputHandle!');
end

Screen('Preference', 'SkipSyncTests', 1);

PsychImaging('PrepareConfiguration');
PsychImaging('AddTask', 'AllViews', 'FlipVertical');

fbRect = Screen('Rect', screenId);
refreshHz = Screen('Framerate', screenId);

if refreshHz == 0
  fbRect = [0 0 1024 600];
  refreshHz = 60;
end

[win, rect] = PsychImaging('OpenWindow', screenId, 0.5, [], [], [], stereomode, multisample, imagingmode, [], [], fbRect);
winfo = Screen('GetWindowInfo', win)
if IsLinux && ~isFullscreen
  outputHandle = uint64(winfo.SysWindowHandle);
end

if ~isempty(winfo.GLDeviceUUID)
  targetUUID = winfo.GLDeviceUUID
else
  targetUUID = zeros(1, 16, 'uint8');
end

% Skip sync tests after 'OpenWindow' so we get the refresh calibration etc. to run:
Screen('Preference', 'SkipSyncTests', 2);

PsychVulkanCore('Verbosity', 4);
count = PsychVulkanCore('GetCount')

winHandle = dec2hex(outputHandle)

% All: gpuindex
% Linux:  RandR output XID fuer RandR output leasing -> Vulkan fullscreen exclusive direct mode display handle.
%         X11 window handle fuer X11/Windowed mode.
%         width x height in pixels for video mode selection, also refreshHz. ==> Could be window rect in desktop coords.
%         -> No OpenGL / GLX stuff on Vulkan side!
%
% Windows: window rect.
%
%[vwin, width, height, glformat, memorysize, interoptexhandle, interopsemaphore] = PsychVulkanCore('OpenWindow', gpuIndex, isFullscreen, rect, outputHandle, refreshHz, colorSpace, colorFormat, flags)

try
  if IsLinux && isFullscreen && strcmp(winfo.DisplayCoreId, 'NVidia')
    system(sprintf('xrandr --screen %i --output %s --off ; sleep 1', screenId, output.name));
  end

  vwin = PsychVulkanCore('OpenWindow', gpuIndex, targetUUID, isFullscreen, screenId, fbRect, outputHandle, hdrMode, colorPrecision, refreshHz, colorSpace, colorFormat, flags)
  [interopObjectHandle, allocationSize, formatSpec, tilingMode, memoryOffset, width, height] = PsychVulkanCore('GetInteropHandle', vwin)
catch
  %system('xrandr --output DP-0 --set ''non-desktop'' 0');
end

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
    error('Unknown formatSpec provided!');
end

if tilingMode
  tilingMode = GL.OPTIMAL_TILING_EXT;
else
  tilingMode = GL.LINEAR_TILING_EXT;
end

Screen('Hookfunction', win, 'ImportDisplayBufferInteropMemory', [], 0, interopObjectHandle, allocationSize, internalFormat, tilingMode, memoryOffset, width, height);

cmdString = sprintf('VulkanThrowaway(0, %i);', win);
Screen('Hookfunction', win, 'AppendMFunction', 'LeftFinalizerBlitChain', 'Vulkan Mono commit Operation', cmdString);
Screen('Hookfunction', win, 'Enable', 'LeftFinalizerBlitChain');

cmdString = sprintf('VulkanThrowaway(1, %i, %i, IMAGINGPIPE_FLIPTWHEN, IMAGINGPIPE_FLIPVBLSYNCLEVEL);', win, vwin);
Screen('Hookfunction', win, 'AppendMFunction', 'PreSwapbuffersOperations', 'Vulkan Present Operation', cmdString);
Screen('Hookfunction', win, 'Enable', 'PreSwapbuffersOperations');

cmdString = sprintf('VulkanThrowaway(2, %i);', vwin);
Screen('Hookfunction', win, 'PrependMFunction', 'CloseOnscreenWindowPostGLShutdown', 'Vulkan cleanup', cmdString);
Screen('Hookfunction', win, 'Enable', 'CloseOnscreenWindowPostGLShutdown');

i = 0;
KbReleaseWait;
tVbl = GetSecs;
tOnset = nan(1,1000);

GL.FRAMEBUFFER_FLIP_Y_MESA = hex2dec('8BBB');
%glFramebufferParameteri(GL.DRAW_FRAMEBUFFER, GL.FRAMEBUFFER_FLIP_Y_MESA, 1);

while ~KbCheck
  i = i + 1;
  Screen('FillRect', win, 0.5);
  Screen('DrawText', win, sprintf('Hello %i', i), 100, 100);
  tWhen = tVbl + 0.0;
  doTimestamp = [];
  [tVbl, tOnset(mod(i, length(tOnset)) + 1)] = Screen('Flip', win, tWhen, [], doTimestamp);
end

Screen('CloseAll');

if IsLinux && isFullscreen && strcmp(winfo.DisplayCoreId, 'NVidia')
  system(sprintf('xrandr --screen %i --output %s --auto', screenId, output.name));
end

tOnset = diff(tOnset) * 1000;
tOnset = tOnset(find(tOnset >= 0));
avg = median(tOnset)
%plot(tOnset);
