function varargout = PsychOculusVR(cmd, varargin)
% PsychOculusVR - A high level driver for Oculus VR hardware.
%
% Usage:
%
% hmd = PsychOculusVR('AutoSetupDefaultHMD');
% - Auto-Detect 1st connected Oculus VR HMD, open it, set it up
% with sane default rendering and display parameters and generate
% a PsychImaging('AddTask', ...) line to setup Psychtoolboxs
% imaging pipeline for proper rendering on the HMD. This will also
% cause the device connection and VR driver to be auto-closed /
% auto-shutdown as soon as the onscreen window which displays on
% the HMD is closed. Returns the 'hmd' handle of the HMD on success.
% If no VR HMD is connected, it will open an emulated/simulated one
% for basic testing and debugging.
%
%
% hmd = PsychOculusVR('Open' [, deviceIndex], ...);
% - Open HMD with index 'deviceIndex'. See PsychOculusVRCore Open?
% for help on additional parameters.
%
%
% PsychOculusVR('SetAutoClose', hmd, mode);
% - Set autoclose mode for HMD with handle 'hmd'. 'mode' can be
% 0 (this is the default) to not do anything special. 1 will close
% the HMD 'hmd' when the onscreen window is closed which displays
% on the HMD. 2 will do the same as 1, but close all open HMDs and
% shutdown the complete driver and Oculus runtime - a full cleanup.
%
%
% isOpen = PsychOculusVR('IsOpen', hmd);
% - Returns 1 if 'hmd' corresponds to an open HMD, 0 otherwise.
%
%
% PsychOculusVR('SetupRenderingParameters', hmd)
% - Query the HMD 'hmd' for its properties and setup internal rendering
% parameters in preparation for opening an onscreen window with PsychImaging
% to display properly on the HMD.
%
%
% [width, height] = PsychOculusVR('GetClientRenderbufferSize', hmd);
% - Retrieve recommended size in pixels [width x height] of the client
% renderbuffer for each eye for rendering to the HMD. Returns parameters
% previously computed by PsychOculusVR('SetupRenderingParameters', hmd).
%
%

% History:
% 07-Sep-2015  mk   Written.

% Global GL handle for access to OpenGL constants needed in setup:
global GL;

persistent hmd;

if nargin < 1 || isempty(cmd)
  help PsychOculusVR;
  fprintf('\n\nAlso available are functions from PsychOculusVRCore:\n');
  PsychOculusVRCore;
  return;
end

% Autodetect first connected HMD and open a connection to it. Open a
% emulated one, if none can be detected. Perform basic setup with
% default configuration, create a proper PsychImaging task.
if strcmpi(cmd, 'AutoSetupDefaultHMD')
  % Check if at least one Oculus HMD is connected and available:
  if PsychOculusVR('GetCount') > 0
    % Yes. Open and initialize connection to first detected HMD:
    fprintf('PsychOculusVR: Opening the first connected Oculus VR headset.\n');
    oculus = PsychOculusVR('Open', 0);
  else
    % No. Open an emulated/simulated HMD for basic testing and debugging:
    fprintf('PsychOculusVR: No Oculus HMD detected. Opening a simulated HMD.\n');
    oculus = PsychOculusVR('Open', -1);
  end

  % Trigger an automatic device close + full driver shutdown at
  % onscreen window close for the HMD display window:
  PsychOculusVR('SetAutoClose', oculus, 2);

  % Setup default rendering parameters:
  PsychOculusVR('SetupRenderingParameters', oculus);

  % Add a PsychImaging task to use this HMD with the next opened onscreen window:
  PsychImaging('AddTask', 'General', 'UseOculusVRHMD', oculus);

  % Return the device handle:
  varargout{1} = oculus;

  % Ready.
  return;
end

if strcmpi(cmd, 'SetAutoClose')
  handle = varargin{1};

  if ~PsychOculusVR('IsOpen', handle)
    error('PsychOculusVR:SetAutoClose: Specified handle does not correspond to an open HMD!');
  end

  % Assign autoclose flag:
  hmd{handle}.autoclose = varargin{2};

  return;
end

% Open a HMD:
if strcmpi(cmd, 'Open')
  % Hack to make sure the VR runtime detects the HMD on a secondary X-Screen:
  if IsLinux && ~IsWayland && length(Screen('Screens')) > 1
    setenv('DISPLAY', sprintf(':0.%i', max(Screen('Screens'))));
  end

  handle = PsychOculusVRCore('Open', varargin{:});
  hmd{handle}.open = 1;

  % Default autoclose flag to "no autoclose":
  hmd{handle}.autoclose = 0;

  varargout{1} = handle;
  return;
end

if strcmpi(cmd, 'IsOpen')
  handle = varargin{1};
  if (length(hmd) >= handle) && (handle > 0) && hmd{handle}.open
    varargout{1} = 1;
  else
    varargout{1} = 0;
  end
  return;
end

if strcmpi(cmd, 'Close')
  if length(varargin) > 0 && ~isempty(varargin{1})
    % Close a specific hmd device:
    handle = varargin{1};
    if (length(hmd) >= handle) && (handle > 0) && hmd{handle}.open
      PsychOculusVRCore('Close', handle);
      hmd{handle}.open = 0;
    end
  else
    % Shutdown whole driver:
    PsychOculusVRCore('Close');
    hmd = [];
  end

  return;
end

if strcmpi(cmd, 'SetupRenderingParameters')
  handle = varargin{1};

  % Get optimal client renderbuffer size - the size of our virtual framebuffer for left eye:
  [hmd{handle}.rbwidth, hmd{handle}.rbheight, hmd{handle}.fovTanPort] = PsychOculusVR ('GetFovTextureSize', handle, 0, varargin{2:end});

  % Get optimal client renderbuffer size - the size of our virtual framebuffer for right eye:
  [hmd{handle}.rbwidth, hmd{handle}.rbheight, hmd{handle}.fovTanPort] = PsychOculusVR ('GetFovTextureSize', handle, 1, varargin{2:end});

  return;
end

if strcmpi(cmd, 'GetClientRenderbufferSize')
  handle = varargin{1};
  varargout{1} = [hmd{handle}.rbwidth, hmd{handle}.rbheight];
  return;
end

if strcmpi(cmd, 'PerformPostWindowOpenSetup')

  % Must have global GL constants:
  if isempty(GL)
    varargout{1} = 0;
    warning('PTB internal error in PsychOculusVR: GL struct not initialized?!?');
    return;
  end

  % Oculus device handle:
  handle = varargin{1};
  
  % Onscreen window handle:
  win = varargin{2};

  % Compute effective size of per-eye input buffer for undistortion render.
  % This is the size of the imaging pipelines processedDrawbufferFBO's, aka
  % usually the inputBufferFBO's, aka whatever is the blit target for the
  % panelfitter - or the size of the actual drawbufferFBO if neither the
  % panelfitter nor multisample anti-aliasing resolve is needed.
  %
  % In our current implementation we allocate said buffer to twice the horizontal
  % size of the real framebuffer, ie., twice the panel width size of the
  % HMD, as that should be plenty for all typical use cases - and is also the
  % maximum possible with the current Screen imaging pipeline:
  [hmd{handle}.inputWidth, hmd{handle}.inputHeight] = Screen('WindowSize', win, 1);
  hmd{handle}.inputWidth = hmd{handle}.inputWidth * 2;
  hmd{handle}.inputHeight = hmd{handle}.inputHeight * 1;

  % Query undistortion parameters for left eye view:
  [hmd{handle}.rbwidth, hmd{handle}.rbheight, vx, vy, vw, vh, ptx, pty, hsx, hsy, hsz, meshVL, meshIL, uvScale(1), uvScale(2), uvOffset(1), uvOffset(2)] = PsychOculusVR ('GetUndistortionParameters', handle, 0, hmd{handle}.inputWidth, hmd{handle}.inputHeight, hmd{handle}.fovTanPort);
  hmd{handle}.viewportLeft = [vx, vy, vw, vh];
  hmd{handle}.PixelsPerTanAngleAtCenterLeft = [ptx, pty];
  hmd{handle}.HmdToEyeViewOffsetLeft = [hsx, hsy, hsz];
  hmd{handle}.meshVerticesLeft = meshVL;
  hmd{handle}.meshIndicesLeft = meshIL;
  hmd{handle}.uvScaleLeft = uvScale;
  hmd{handle}.uvOffsetLeft = uvOffset;

  % Init warp matrices to identity, until we get something better from live tracking:
  hmd{handle}.eyeRotStartMatrixLeft = diag([1 1 1 1]);
  hmd{handle}.eyeRotEndMatrixLeft   = diag([1 1 1 1]);

  %scaleL=uvScale
  %offsetL=uvOffset

  % Query parameters for right eye view:
  [hmd{handle}.rbwidth, hmd{handle}.rbheight, vx, vy, vw, vh, ptx, pty, hsx, hsy, hsz, meshVR, meshIR, uvScale(1), uvScale(2), uvOffset(1), uvOffset(2)] = PsychOculusVR ('GetUndistortionParameters', handle, 1, hmd{handle}.inputWidth, hmd{handle}.inputHeight, hmd{handle}.fovTanPort);
  hmd{handle}.viewportRight = [vx, vy, vw, vh];
  hmd{handle}.PixelsPerTanAngleAtCenterRight = [ptx, pty];
  hmd{handle}.HmdToEyeViewOffsetRight = [hsx, hsy, hsz];
  hmd{handle}.meshVerticesRight = meshVR;
  hmd{handle}.meshIndicesRight = meshIR;
  hmd{handle}.uvScaleRight = uvScale;
  hmd{handle}.uvOffsetRight = uvOffset;

  % Init warp matrices to identity, until we get something better from live tracking:
  hmd{handle}.eyeRotStartMatrixRight = diag([1 1 1 1]);
  hmd{handle}.eyeRotEndMatrixRight   = diag([1 1 1 1]);

  %scaleR=uvScale
  %offsetR=uvOffset

  [slot shaderid blittercfg voidptr glsl] = Screen('HookFunction', win, 'Query', 'StereoCompositingBlit', 'StereoCompositingShaderAnaglyph');
  if slot == -1
    varargout{1} = 0;
    warning('Either the imaging pipeline is not enabled for given onscreen window, or it is not switched to Anaglyph stereo mode.');
    return;
  end

  if glsl == 0
    varargout{1} = 0;
    warning('Anaglyph shader is not operational for unknown reason. Sorry...');
    return;
  end

  % Remove old standard anaglyph shader:
  Screen('HookFunction', win, 'Remove', 'StereoCompositingBlit', slot);

  % Build the unwarp mesh display list within the OpenGL context of Screen():
  Screen('BeginOpenGL', win, 1);

  % Left eye setup:
  % ---------------

  % Build a display list that corresponds to the current calibration,
  % drawing the warp-mesh once, so it gets recorded in the display list:
  gldLeft = glGenLists(1);
  glNewList(gldLeft, GL.COMPILE);

  % Caution: Must *copy* the different rows with data into *separate* variables, so
  % the vertex array pointers to the different variables actually point to something
  % persistent! If we'd pass the meshVerticesLeft() subarrays directly to glTexCoordPointer
  % and friends then Octave/Matlab would just create a temporary copy of the extracted
  % rows, OpenGL would retrieve/assign pointers to those temporary copies, but then
  % at the end of a glVertexPointer/glTexCoordPointer call, those temporary copies would
  % go out of scope and Octave/Matlab would potentially garbage collect the variables again
  % *before* the call to glDrawElements permanently records the content of the variables.
  % The net results would be stale/dangling pointers, random data trash getting read from
  % memory and recorded in the display list - and thereby corrupted rendering! This hazard
  % doesn't exist within regular Octave/Matlab scripts, because the interpreter doesn't
  % deal with memory pointers. It is a unique hazard from the combination of C memory
  % pointers for OpenGL and Octave/Matlabs copy-on-write/data-sharing/garbage collection
  % behaviour. When we are at it, lets also cast the data to single() precision floating
  % point, to save some memory:
  vertexpos = single(hmd{handle}.meshVerticesLeft(1:4, :));
  texR = single(hmd{handle}.meshVerticesLeft(5:6, :));
  texG = single(hmd{handle}.meshVerticesLeft(7:8, :));
  texB = single(hmd{handle}.meshVerticesLeft(9:10, :));

  % vertex xy encodes 2D position from rows 1 and 2, z encodes timeWarp interpolation factors
  % from row 3 and w encodes vignette correction factors from row 4:
  glEnableClientState(GL.VERTEX_ARRAY);
  glVertexPointer(4, GL.FLOAT, 0, vertexpos);

  % Need separate texture coordinate sets for the three color channel to encode
  % channel specific color aberration correction sampling:

  % TexCoord set 0 encodes coordinates for the Red color channel:
  glClientActiveTexture(GL.TEXTURE0);
  glEnableClientState(GL.TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL.FLOAT, 0, texR);
  
  % TexCoord set 1 encodes coordinates for the Green color channel:
  glClientActiveTexture(GL.TEXTURE1);
  glEnableClientState(GL.TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL.FLOAT, 0, texG);

  % TexCoord set 2 encodes coordinates for the Blue color channel:
  glClientActiveTexture(GL.TEXTURE2);
  glEnableClientState(GL.TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL.FLOAT, 0, texB);

  % Draw the mesh. This records the content from all the variables persistently into
  % the display list storage, so they can be freed afterwards:
  glDrawElements(GL.TRIANGLES, length(hmd{handle}.meshIndicesLeft), GL.UNSIGNED_SHORT, uint16(hmd{handle}.meshIndicesLeft));

  % Disable stuff, so we can release or recycle the variables:
  glClientActiveTexture(GL.TEXTURE3);
  glDisableClientState(GL.TEXTURE_COORD_ARRAY);

  glClientActiveTexture(GL.TEXTURE2);
  glDisableClientState(GL.TEXTURE_COORD_ARRAY);

  glClientActiveTexture(GL.TEXTURE1);
  glDisableClientState(GL.TEXTURE_COORD_ARRAY);

  glClientActiveTexture(GL.TEXTURE0);
  glDisableClientState(GL.TEXTURE_COORD_ARRAY);

  glDisableClientState(GL.VERTEX_ARRAY);
  
  % Left eye display list done.
  glEndList;

  % Right eye setup:
  % ---------------

  % Build a display list that corresponds to the current calibration,
  % drawing the warp-mesh once, so it gets recorded in the display list:
  gldRight = glGenLists(1);
  glNewList(gldRight, GL.COMPILE);

  vertexpos = single(hmd{handle}.meshVerticesRight(1:4, :));
  texR = single(hmd{handle}.meshVerticesRight(5:6, :));
  texG = single(hmd{handle}.meshVerticesRight(7:8, :));
  texB = single(hmd{handle}.meshVerticesRight(9:10, :));

  % vertex xy encodes 2D position from rows 1 and 2, z encodes timeWarp interpolation factors
  % from row 3 and w encodes vignette correction factors from row 4:
  glEnableClientState(GL.VERTEX_ARRAY);
  glVertexPointer(4, GL.FLOAT, 0, vertexpos);

  % Need separate texture coordinate sets for the three color channel to encode
  % channel specific color aberration correction sampling:

  % TexCoord set 0 encodes coordinates for the Red color channel:
  glClientActiveTexture(GL.TEXTURE0);
  glEnableClientState(GL.TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL.FLOAT, 0, texR);
  
  % TexCoord set 1 encodes coordinates for the Green color channel:
  glClientActiveTexture(GL.TEXTURE1);
  glEnableClientState(GL.TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL.FLOAT, 0, texG);

  % TexCoord set 2 encodes coordinates for the Blue color channel:
  glClientActiveTexture(GL.TEXTURE2);
  glEnableClientState(GL.TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL.FLOAT, 0, texB);

  % Draw the mesh. This records the content from all the variables persistently into
  % the display list storage, so they can be freed afterwards:
  glDrawElements(GL.TRIANGLES, length(hmd{handle}.meshIndicesRight), GL.UNSIGNED_SHORT, uint16(hmd{handle}.meshIndicesRight));

  % Disable stuff, so we can release or recycle the variables:
  glClientActiveTexture(GL.TEXTURE3);
  glDisableClientState(GL.TEXTURE_COORD_ARRAY);

  glClientActiveTexture(GL.TEXTURE2);
  glDisableClientState(GL.TEXTURE_COORD_ARRAY);

  glClientActiveTexture(GL.TEXTURE1);
  glDisableClientState(GL.TEXTURE_COORD_ARRAY);

  glClientActiveTexture(GL.TEXTURE0);
  glDisableClientState(GL.TEXTURE_COORD_ARRAY);

  glDisableClientState(GL.VERTEX_ARRAY);
  
  % Right eye display list done.
  glEndList;

  Screen('EndOpenGL', win);

  % Setup left eye shader:
  glsl = LoadGLSLProgramFromFiles('OculusRiftCorrectionShader');
  glUseProgram(glsl);
  glUniform1i(glGetUniformLocation(glsl, 'Image'), 0);
  glUniform2f(glGetUniformLocation(glsl, 'EyeToSourceUVOffset'), hmd{handle}.uvOffsetLeft(1) * hmd{handle}.inputWidth, hmd{handle}.uvOffsetLeft(2) * hmd{handle}.inputHeight);
  glUniform2f(glGetUniformLocation(glsl, 'EyeToSourceUVScale'), hmd{handle}.uvScaleLeft(1) * hmd{handle}.inputWidth, hmd{handle}.uvScaleLeft(2) * hmd{handle}.inputHeight);
  glUniformMatrix4fv(glGetUniformLocation(glsl, 'EyeRotationStart'), 1, 1, hmd{handle}.eyeRotStartMatrixLeft);
  glUniformMatrix4fv(glGetUniformLocation(glsl, 'EyeRotationEnd'), 1, 1, hmd{handle}.eyeRotEndMatrixLeft);
  glUseProgram(0);

  % Insert it at former position of the old shader:
  posstring = sprintf('InsertAt%iShader', slot);
  
  % xOffset and yOffset encode the viewport location and size for the left-eye vs.
  % right eye view in the shared output window - or the source renderbuffer if both eyes
  % would be rendered into a shared texture. However, the meshes provided by the SDK
  % already encode proper left and right offsets for output, and the inputs are separate
  % textures for left and right eye, so using the offset is not needed. Also our correction
  % shader ignores the modelview matrix which would get updated with the "Offset:%i%i" blittercfg,
  % instead is takes normalized device coordinates NDC directly from the distortion mesh. Iow, not
  % only is xOffset/yOffset not needed, it would also be a no operation due to our specific shader.
  % We leave this here for documentation for now, in case we need to change our ways of doing this.
  %leftViewPort = hmd{handle}.viewportLeft
  blittercfg = sprintf('Blitter:DisplayListBlit:Handle:%i:Bilinear', gldLeft);
  Screen('Hookfunction', win, posstring, 'StereoCompositingBlit', 'OculusVRClientCompositingShaderLeftEye', glsl, blittercfg);

  % Setup right eye shader:
  glsl = LoadGLSLProgramFromFiles('OculusRiftCorrectionShader');
  glUseProgram(glsl);
  glUniform1i(glGetUniformLocation(glsl, 'Image'), 1);
  glUniform2f(glGetUniformLocation(glsl, 'EyeToSourceUVOffset'), hmd{handle}.uvOffsetRight(1) * hmd{handle}.inputWidth, hmd{handle}.uvOffsetRight(2) * hmd{handle}.inputHeight);
  glUniform2f(glGetUniformLocation(glsl, 'EyeToSourceUVScale'), hmd{handle}.uvScaleRight(1) * hmd{handle}.inputWidth, hmd{handle}.uvScaleRight(2) * hmd{handle}.inputHeight);
  glUniformMatrix4fv(glGetUniformLocation(glsl, 'EyeRotationStart'), 1, 1, hmd{handle}.eyeRotStartMatrixRight);
  glUniformMatrix4fv(glGetUniformLocation(glsl, 'EyeRotationEnd'), 1, 1, hmd{handle}.eyeRotEndMatrixRight);
  glUseProgram(0);

  % Insert it at former position of the old shader:
  posstring = sprintf('InsertAt%iShader', slot);
  blittercfg = sprintf('Blitter:DisplayListBlit:Handle:%i:Bilinear', gldRight);
  Screen('Hookfunction', win, posstring, 'StereoCompositingBlit', 'OculusVRClientCompositingShaderRightEye', glsl, blittercfg);

  % Does usercode request auto-closing the HMD or driver when the onscreen window is closed?
  if hmd{handle}.autoclose > 0
    % Attach a window close callback for Device teardown at window close time:
    if hmd{handle}.autoclose == 2
      % Shutdown driver completely:
      Screen('Hookfunction', win, 'AppendMFunction', 'CloseOnscreenWindowPostGLShutdown', 'Shutdown window callback into PsychOculusVR driver.', 'PsychOculusVR(''Close'');');
    else
      % Only close this HMD:
      Screen('Hookfunction', win, 'AppendMFunction', 'CloseOnscreenWindowPostGLShutdown', 'Shutdown window callback into PsychOculusVR driver.', sprintf('PsychOculusVR(''Close'', %i);', handle));
    end

    Screen('HookFunction', win, 'Enable', 'CloseOnscreenWindowPostGLShutdown');
  end

  % Return success result code 1:
  varargout{1} = 1;
  return;
end

% 'cmd' so far not dispatched? Let's assume it is a command
% meant for PsychOculusVRCore:
[ varargout{1:nargout} ] = PsychOculusVRCore(cmd, varargin{:});
return;

end
