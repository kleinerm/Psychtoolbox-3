function ImagingPipelineExternalConsumerTest
  AssertOpenGL;
  close all;

  %InitializeMatlabOpenGL;
  Screen('Preference', 'SkipSyncTests', 2);
  %Screen('Preference', 'Verbosity', 10);
  multisample = 0;
  stereomode = 12;
  imagingmode = kPsychNeedFinalizedFBOSinks + kPsychSinkIsMSAACapable + kPsychUseExternalSinkTextures;
  PsychImaging('PrepareConfiguration');
  PsychImaging('AddTask', 'General', 'UseVirtualFramebuffer');
  %PsychImaging('AddTask', 'General', 'UseDisplayRotation', 90);
  %PsychImaging('AddTask', 'General', 'UsePanelFitter', [200 200], 'Centered');
  if stereomode == 0
    %PsychImaging('AddTask', 'FinalFormatting', 'DisplayColorCorrection', 'SimpleGamma');
  else
    %PsychImaging('AddTask', 'LeftView', 'DisplayColorCorrection', 'SimpleGamma');
    %PsychImaging('AddTask', 'RightView', 'DisplayColorCorrection', 'SimpleGamma');
  end
  %PsychImaging('AddTask', 'General', 'EnablePseudoGrayOutput');
  [w, wrect] = PsychImaging('Openwindow', 0, 128, [0 0 300 300], [], [], stereomode, multisample, imagingmode);

  if 0 && stereomode > 0
    PsychColorCorrection('SetEncodingGamma', w, 1/2.8, 'LeftView');
    PsychColorCorrection('SetEncodingGamma', w, 1/0.5, 'RightView');
  end
  winfo = Screen('GetWindowInfo', w)

  Screen('SelectStereoDrawBuffer', w, 0);
  Screen('FillOval', w, [255 0 0]);
  DrawFormattedText(w, 'Left', 'center', 'center', [255 255 0]);
  dl = Screen('GetImage', w, [], 'drawBuffer');

  if stereomode > 0
    Screen('SelectStereoDrawBuffer', w, 1);
    Screen('FillOval', w, [0 255 0]);
    DrawFormattedText(w, 'Right', 'center', 'center', [255 255 0]);
    dr = Screen('GetImage', w, [], 'drawBuffer');
  end

  Screen('Flip', w, [], 2);
  if stereomode > 0
    bl = Screen('GetImage', w, [0 0 300 300], 'backLeftBuffer');
    br = Screen('GetImage', w, [0 0 300 300], 'backRightBuffer');
  else
    bl = Screen('GetImage', w, [0 0 300 300], 'backBuffer');
  end
  KbStrokeWait;
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
