function DrawingIntoTexturesTest(imagingmode)

if IsOctave
  pkg load image;
end

AssertOpenGL;

screenid = max(Screen('Screens'));

if nargin < 1
    imagingmode = [];
end

if isempty(imagingmode)
    imagingmode = 0;
end

if imagingmode > 0
    imagingmode = kPsychNeedFastBackingStore;
    floatflag = 2;
else
    imagingmode = 0;
    floatflag = 0;
end

% Open window, with or without imaging pipeline:
win = Screen('OpenWindow', screenid, 255, [], [], [], [], [], imagingmode);
Screen('Blendfunction', win, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

for floatprec =0:floatflag
    for layers=1:4
        fprintf('Testing Drawing into %i layer textures of precision mode %i...\n', layers, floatprec);

        inimg = imread([PsychtoolboxRoot 'PsychDemos/konijntjes1024x768.jpg']);
        switch layers
            case 1,
                teximg = rgb2gray(inimg);
            case 2,
                teximg = randn(size(inimg, 1), size(inimg, 2), 2) * 255;
                teximg(:,:,1) = rgb2gray(inimg);
            case 3,
                teximg = inimg;
            case 4,
                teximg = randn(size(inimg, 1), size(inimg, 2), 4) * 255;
                teximg(:,:,1:3) = inimg;
        end

        if (floatprec>0)
            teximg = double(teximg)/255.0;
        end
        
        tex = Screen('MakeTexture', win, teximg, [], [], floatprec);

        DrawFormattedText(tex, ['Everything is fine with ' num2str(layers) ' layers of precision' num2str(floatprec) '...'], 'center', 'center');

        Screen('DrawTexture', win, tex);
        Screen('Flip', win);

        while KbCheck; end;
        KbWait;
    end
end

Screen('CloseAll');
