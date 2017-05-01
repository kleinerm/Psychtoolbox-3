function ImagingPipelineExternalConsumerTest(cmd, varargin)
% ImagingPipelineExternalConsumerTest - Test code for imaging pipe output redirection.
%
% To proof-of-concept test the imaging pipelines mechanisms to redirect
% final rendered output to an external consumer or video sink, instead of
% displaying in the onscreen window.
%
% This test not really useful for end users, just for internal
% development and debugging. Does not work on all platforms or
% runtimes.
%
% History:
% Mar-2017  mk Written.

  global GL;
  global w;
  global stereomode;
  global oldglleft;
  global oldglright;
  global curtex;
  global leftTextures;
  global rightTextures;

  if nargin > 0
    if cmd == 0
      ProducerConsumer(varargin{:});
      return;
    end

    if cmd == 1
      ReleaseProducer;
      return;
    end

    if cmd == 2
      PassiveConsumer;
      return;
    end
  end

  PsychDefaultSetup(2);
  close all;

  %InitializeMatlabOpenGL;
  Screen('Preference', 'ConserveVRAM', 524288);
  Screen('Preference', 'SkipSyncTests', 2);
  Screen('Preference', 'Verbosity', 5);
  injectexternal = 1;
  multisample = 0;
  stereomode = 12;
  doGamma = 0;
  doPost = 0;
  imagingmode = kPsychNeedFinalizedFBOSinks + kPsychSinkIsMSAACapable;
  if injectexternal
    imagingmode = imagingmode + kPsychUseExternalSinkTextures;
  end

  PsychImaging('PrepareConfiguration');
  PsychImaging('AddTask', 'General', 'UseVirtualFramebuffer');
  %PsychImaging('AddTask', 'General', 'UseDisplayRotation', 90);
  %PsychImaging('AddTask', 'General', 'UsePanelFitter', [200 200], 'Centered');

  % Sample image processing step:
  if doGamma
    if stereomode == 0
      PsychImaging('AddTask', 'FinalFormatting', 'DisplayColorCorrection', 'SimpleGamma');
    else
      PsychImaging('AddTask', 'LeftView', 'DisplayColorCorrection', 'SimpleGamma');
      PsychImaging('AddTask', 'RightView', 'DisplayColorCorrection', 'SimpleGamma');
    end
  end

  % Sample post processing step:
  if doPost
    PsychImaging('AddTask', 'General', 'EnablePseudoGrayOutput');
  end

  [w, wrect] = PsychImaging('Openwindow', 0, 0.5, [0 0 300 300], [], [], stereomode, multisample, imagingmode);

  % Sample setup code for external consumer creating/providing the
  % backing textures as a triple-buffered texture swapchain:
  if injectexternal
    % Attach 1st buffer as initial render buffer:
    curtex = 0;

    [oldglleft, oldglright, target, format, multisample, width, height] = Screen('Hookfunction', w, 'GetDisplayBufferTextures');

    % Create triple-buffer textures as swapchain:
    leftTextures = glGenTextures(3);
    for i=1:3
      glBindTexture(target, leftTextures(i));
      if target ~= GL.TEXTURE_2D_MULTISAMPLE
        glTexParameteri(target, GL.TEXTURE_MIN_FILTER, GL.NEAREST);
        glTexParameteri(target, GL.TEXTURE_MAG_FILTER, GL.NEAREST);
        glTexImage2D(target, 0, format, width, height, 0, GL.RGBA, GL.UNSIGNED_BYTE, 0);
      else
        glTexImage2DMultisample(target, multisample, format, width, height, 1);
      end
    end
    glBindTexture(target, 0);

    if stereomode > 0
      rightTextures = glGenTextures(3);
      for i=1:3
        glBindTexture(target, rightTextures(i));
        if target ~= GL.TEXTURE_2D_MULTISAMPLE
          glTexParameteri(target, GL.TEXTURE_MIN_FILTER, GL.NEAREST);
          glTexParameteri(target, GL.TEXTURE_MAG_FILTER, GL.NEAREST);
          glTexImage2D(target, 0, format, width, height, 0, GL.RGBA, GL.UNSIGNED_BYTE, 0);
        else
          glTexImage2DMultisample(target, multisample, format, width, height, 1);
        end
      end
      glBindTexture(target, 0);

      Screen('Hookfunction', w, 'SetDisplayBufferTextures', '', double(leftTextures(curtex + 1)), double(rightTextures(curtex + 1)), target, format, multisample, width, height);
    else
      Screen('Hookfunction', w, 'SetDisplayBufferTextures', '', double(leftTextures(curtex + 1)), [], target, format, multisample, width, height);
    end

    if 1
      Screen('Hookfunction', w, 'AppendMFunction', 'PreSwapbuffersOperations', 'Snapshotter', 'ImagingPipelineExternalConsumerTest(0, IMAGINGPIPE_FLIPTWHEN, IMAGINGPIPE_FLIPDONTCLEAR, IMAGINGPIPE_FLIPVBLSYNCLEVEL);');
    else
      Screen('Hookfunction', w, 'AppendMFunction', 'LeftFinalizerBlitChain', 'Snapshotter', 'ImagingPipelineExternalConsumerTest(0, IMAGINGPIPE_FLIPTWHEN, IMAGINGPIPE_FLIPDONTCLEAR, IMAGINGPIPE_FLIPVBLSYNCLEVEL);');
      Screen('Hookfunction', w, 'Enable', 'LeftFinalizerBlitChain');
    end
    Screen('Hookfunction', w, 'PrependMFunction', 'CloseOnscreenWindowPreGLShutdown', 'ProducerConsumerTearDown', 'ImagingPipelineExternalConsumerTest(1)');
    Screen('Hookfunction', w, 'Enable', 'CloseOnscreenWindowPreGLShutdown');
  else
    Screen('Hookfunction', w, 'AppendMFunction', 'PreSwapbuffersOperations', 'Snapshotter', 'ImagingPipelineExternalConsumerTest(2)');
  end
  Screen('Hookfunction', w, 'Enable', 'PreSwapbuffersOperations');

  if doGamma && stereomode > 0
    PsychColorCorrection('SetEncodingGamma', w, 1/2.8, 'LeftView');
    PsychColorCorrection('SetEncodingGamma', w, 1/0.5, 'RightView');
  end
  winfo = Screen('GetWindowInfo', w)

  Screen('SelectStereoDrawBuffer', w, 0);
  Screen('FillOval', w, [1 0 0]);
  DrawFormattedText(w, 'Left', 'center', 'center', [1 1 0]);
  dl = Screen('GetImage', w, [], 'drawBuffer');

  if stereomode > 0
    Screen('SelectStereoDrawBuffer', w, 1);
    Screen('FillOval', w, [0 1 0]);
    DrawFormattedText(w, 'Right', 'center', 'center', [1 1 0]);
    dr = Screen('GetImage', w, [], 'drawBuffer');
  end

  Screen('Flip', w, [], 0);
  KbStrokeWait;
  
  while ~KbCheck
    Screen('SelectStereoDrawBuffer', w, 0);
    Screen('FillOval', w, [1 0 0]);
    DrawFormattedText(w, sprintf('%0.3f', GetSecs), 'center', 'center', [1 1 0]);
    if stereomode > 0
      Screen('SelectStereoDrawBuffer', w, 1);
      Screen('FillOval', w, [0 1 0]);
      DrawFormattedText(w, sprintf('%0.3f', GetSecs), 'center', 'center', [1 1 0]);
    end

    Screen('Flip', w, GetSecs + 0.001, [], round(mod(GetSecs, 2)));
  end
  
  if stereomode > 0
    bl = Screen('GetImage', w, [0 0 300 300], 'backLeftBuffer');
    br = Screen('GetImage', w, [0 0 300 300], 'backRightBuffer');
  else
    bl = Screen('GetImage', w, [0 0 300 300], 'backBuffer');
  end

  sca;

  figure;
  imshow(dl);
  title('Left drawBuffer drawBufferFBO[0]');
  
  if stereomode > 0
    figure;
    imshow(dr);
    title('Right drawBuffer drawBufferFBO[1]');
  end

  figure;
  imshow(bl);
  title('Back left buffer - finalizedFBO[0]');
  if stereomode > 0
    figure;
    imshow(br);
    title('Back right buffer - finalizedFBO[1]');
  end
end

function PassiveConsumer
  global GL;
  global w;
  [glleft, glright, target, format, multisample, width, height] = Screen('Hookfunction', w, 'GetDisplayBufferTextures')
  glEnable(GL.TEXTURE_2D);
  glBindTexture(GL.TEXTURE_2D, glleft);
%  pixels = glGetTexImage(GL.TEXTURE_2D, 0, GL.RGBA, GL.UNSIGNED_BYTE);
%  fbo = glGetIntegerv(GL.DRAW_FRAMEBUFFER_BINDING)
  glBegin(GL.QUADS);
  glColor4f(1,1,1,1);
  glTexCoord2f(0,1);
  glVertex2i(0,0);
  glTexCoord2f(1,1);
  glVertex2i(width,0);
  glTexCoord2f(1,0);
  glVertex2i(width,height);
  glTexCoord2f(0,0);
  glVertex2i(0,height);
  glEnd();
  glBindTexture(GL.TEXTURE_2D, 0);
  glDisable(GL.TEXTURE_2D);
%  imshow(squeeze(pixels(2, :, :))');
end

function ProducerConsumer(when, dontclear, synclevel)
  global GL;
  global w;
  global stereomode;
  global curtex;
  global leftTextures;
  global rightTextures;

  if nargin >= 3
    fprintf('synclevel %i : dontclear=%i : tWhen = %f secs.\n', synclevel, dontclear, when);
  end

  % Advance to next buffer in swapchain:
  curtex = mod(curtex + 1, length(leftTextures));

  % Dequeue old texture with brand-new rendered content, enqueue next idle buffer in chain:
  fbo1 = glGetIntegerv(GL.DRAW_FRAMEBUFFER_BINDING)
  if stereomode > 0
    [glleft, glright, target, format, multisample, width, height] = Screen('Hookfunction', w, 'SetDisplayBufferTextures', [], double(leftTextures(curtex + 1)), double(rightTextures(curtex + 1)))
  else
    [glleft, glright, target, format, multisample, width, height] = Screen('Hookfunction', w, 'SetDisplayBufferTextures', [], double(leftTextures(curtex + 1)))
  end
  if target == GL.TEXTURE_2D_MULTISAMPLE
    % Can not handle MSAA textures with fixed function pipeline.
    return;
  end

  glEnable(GL.TEXTURE_2D);
  glBindTexture(GL.TEXTURE_2D, glleft);
%  pixels = glGetTexImage(GL.TEXTURE_2D, 0, GL.RGBA, GL.UNSIGNED_BYTE);
  fbo2 = glGetIntegerv(GL.DRAW_FRAMEBUFFER_BINDING)
  glBindFramebuffer(GL.DRAW_FRAMEBUFFER, 0);
  glBegin(GL.QUADS);
  glColor4f(1,1,1,1);
  glTexCoord2f(0,1);
  glVertex2i(0,0);
  glTexCoord2f(1,1);
  glVertex2i(width,0);
  glTexCoord2f(1,0);
  glVertex2i(width,height);
  glTexCoord2f(0,0);
  glVertex2i(0,height);
  glEnd();

  if stereomode > 0
    glBindTexture(GL.TEXTURE_2D, glright);
    glBegin(GL.QUADS);
    glColor4f(1,1,1,1);
    glTexCoord2f(0,1);
    glVertex2i(width/2,height/2);
    glTexCoord2f(1,1);
    glVertex2i(width,height/2);
    glTexCoord2f(1,0);
    glVertex2i(width,height);
    glTexCoord2f(0,0);
    glVertex2i(width/2,height);
    glEnd();
  end

  
  glBindTexture(GL.TEXTURE_2D, 0);
  glDisable(GL.TEXTURE_2D);
%  imshow(squeeze(pixels(2, :, :))');
end

function ReleaseProducer
  global GL;
  global w;
  global stereomode;
  global oldglleft;
  global oldglright;
  global leftTextures;
  global rightTextures;

  % Reattach old backing textures, so it can get properly destroyed during pipeline shutdown:
  Screen('Hookfunction', w, 'SetDisplayBufferTextures', '', oldglleft, oldglright);

  % Delete our swapchain textures:
  glDeleteTextures(length(leftTextures), leftTextures);
  if stereomode > 0
    glDeleteTextures(length(leftTextures), leftTextures);
  end
end
