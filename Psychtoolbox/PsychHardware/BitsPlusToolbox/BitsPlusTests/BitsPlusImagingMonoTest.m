function BitsPlusImagingMonoTest(testdrawingcmds)
% BitsPlusImagingMonoTest([testdrawingcmds])
%
% Test of built-in Mono++ output conversion shader of PTB
% imaging pipeline. This is meant to test Bits++ Mono++
% conversion of GPU against reference implementation of
% Matlab BitsPlus toolbox.
%
% First the stimulus is generated via Matlab conversion routine.
% Then it is drawn via PTB imaging pipeline conversion.
% Then the PTB imaging result is read back and compared against
% the result of the Matlab implementation and the maximum error
% is returned. This error should be zero.
%
% The optional parameter 'testdrawingcmds' specs if pure texture
% mapping should be tested, or if a combo of texture mapping and
% Screen 2D drawing commands gets tested (1).
%
% THIS TEST IS NOT FINISHED. EARLY BETA.
%

if nargin < 1
    testdrawingcmds = 0;
end

testdrawingcmds

% Define screen
whichScreen=max(Screen('Screens'));

% Increase level of verbosity so we get all the debug-messages:
%Screen('Preference', 'Verbosity', 6);

% Open window, fill frame buffer with black background.
% Enable imaging pipeline with HDR framebuffer:

%imagingmode = mor(kPsychNeedFastBackingStore, kPsychNeedOutputConversion, kPsychNeed16BPCFixed);

imagingmode = 0;

[window,screenRect] = BitsPlusPlus('OpenWindowMono++', whichScreen, 0, [], [], [], [], [], imagingmode);

% [window,screenRect] = Screen(whichScreen,'OpenWindow',0,[],[],[],[],[], imagingmode);
% 
% % First load the graphics hardwares gamma table with an identity mapping,
% % so it doesn't interfere with Bits++
% LoadIdentityClut(window);
% 
% % Set color range to 0 to 65535:
% %Screen('ColorRange', window, 2^16 - 1);
% Screen('ColorRange', window, 1);
% 
% % Setup Bits++ Mono++ formatting shader:
% % Now enable output formatter hook chain and load them with the special Bits++
% % Mono++ data formatting shader:
% 
% shader = LoadGLSLProgramFromFiles('Bits++_Mono++_FormattingShader', 1);
% Screen('Preference', 'Enable3DGraphics', 0);
% glUseProgram(shader);
% glUniform1i(glGetUniformLocation(shader, 'Image'), 0);
% %glUniform1i(glGetUniformLocation(shader, 'moduloLUT'), 1);
% glUseProgram(0);
% 
% % global GL;
% %
% % moduloLUTtex = glGenTextures(1);
% % glBindTexture(GL.TEXTURE_1D, moduloLUTtex);
% % moduloLUT=uint8(0:255);
% % glTexImage1D(GL.TEXTURE_1D, 0, GL.LUMINANCE8, 256, 0, GL.LUMINANCE, GL.UNSIGNED_BYTE, moduloLUT);
% %
% % % Make sure we use nearest neighbour sampling:
% % glTexParameteri(GL.TEXTURE_1D, GL.TEXTURE_MIN_FILTER, GL.NEAREST);
% % glTexParameteri(GL.TEXTURE_1D, GL.TEXTURE_MAG_FILTER, GL.NEAREST);
% %
% % % And that we wrap around, aka repeat:
% % glTexParameteri(GL.TEXTURE_1D, GL.TEXTURE_WRAP_S, GL.REPEAT);
% % glBindTexture(GL.TEXTURE_1D, 0);
% 
% %Screen('HookFunction', window, 'PrependShader', 'FinalOutputFormattingBlit', 'Mono++ output formatting shader for CRS Bits++', shader, ['TEXTURE1D(1)=' num2str(moduloLUTtex)]);
% Screen('HookFunction', window, 'PrependShader', 'FinalOutputFormattingBlit', 'Mono++ output formatting shader for CRS Bits++', shader);
% Screen('HookFunction', window, 'Enable', 'FinalOutputFormattingBlit');

s=256
dstrect = CenterRect([0 0 s s], Screen('Rect', window));

if testdrawingcmds
    colorval = 65462 / 65535;
    inimg=colorval * ones(256, 256);
else
    inimg=reshape(double(linspace(0, 2^16 - 1, 2^16)), 256, 256)' / (2^16 - 1);
end

tic;
monoImage = BitsPlusPackMonoImage(inimg * 65535);
toc
msize=size(monoImage)
mclass=class(monoImage)
for i=1:10

    tex = Screen('MakeTexture', window, inimg, [], [], 2);
    Screen('DrawTexture', window, tex, [], dstrect, [], 0);
    Screen('Close', tex);

    if testdrawingcmds
        Screen(window,'FillOval', colorval, dstrect - [0 0 100 100]);
    end
    
    glFinish;
    tic
    Screen('DrawingFinished', window);
    glFinish;
    toc

    convImage = Screen('GetImage', window, dstrect,'backBuffer');
    Screen('Flip', window, 0, 0, 2);

    rm=max(max(monoImage(:,:,1)))
    gm=max(max(monoImage(:,:,2)))
    bm=max(max(monoImage(:,:,3)))
%    imwrite(monoImage, '/Users/kleinerm/Desktop/MEncoded.bmp');
    rc=max(max(convImage(:,:,1)))
    gc=max(max(convImage(:,:,2)))
    bc=max(max(convImage(:,:,3)))
%    imwrite(convImage, '/Users/kleinerm/Desktop/GEncoded.bmp');
    
    diffred = abs(double(monoImage(:,:,1)) - double(convImage(:,:,1)));
    diffgreen = abs(double(monoImage(:,:,2)) - double(convImage(:,:,2)));

    mdr = max(max(diffred));
    mdg = max(max(diffgreen));
    
    fprintf('Maximum difference: red (hi) = %f green (low)= %f\n', mdr, mdg);
    
    if mdr>0 || mdg>0
        close all;
        imagesc(diffred);
        figure;
        imagesc(diffgreen);
    end
end
KbWait;
% Close the window.
Screen('CloseAll');

% Blank the screen
% BitsPlusBlank(whichScreen);
