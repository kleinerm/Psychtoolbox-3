function testGLInterop

Screen('Preference','SkipSyncTests', 2);
PsychImaging('PrepareConfiguration');
PsychImaging('AddTask', 'General', 'FloatingPoint32Bit');
w = PsychImaging('OpenWindow', 0, 0);

tex = ones(200, 200, 4);
t = Screen('MakeTexture', w, tex, [], [], 2);

% Read our beloved bunny image from filesystem:
bunnyimg = imread([PsychtoolboxRoot 'PsychDemos/konijntjes1024x768.jpg']);
bunnyimg(:,:,4) = 255;

% Maketexture in float precision with upright texture orientation:
bunnytex = Screen('MakeTexture', w, double(bunnyimg)/255, [], [], 2, 1);

%bunnytex = Screen('Openoffscreenwindow', w, [1 1 0 0], [0 0 1024 768], 128);
%DrawFormattedText(bunnytex, 'HELLO WORLD!', 'center', 'center', [255 0 0]);

t = bunnytex;

Screen('DrawTexture', w, t);

if 0
    T = GPUTypeFromToGL(0, w, 2);
end
Screen('Flip', w);


if 1
    if 1
        % Test high-level interface for pure mortals:
        T = GPUTypeFromToGL(0, bunnytex);
    else
        % Test low-level OpenGL object interface which does without any calls
        % into Screen() -- Important for interop with non-ptb code and for use
        % within imaging pipeline, ie., within Screen's execution context, as
        % Screen is currently non-reentrant.
        [texstruct.glhandle, texstruct.gltarget] = Screen('GetOpenGLTexture', w, t);
        [texstruct.width, texstruct.height] = Screen('Windowsize', t);
        texstruct.bpp = Screen('Pixelsize', t) / 8;
        texstruct.nrchannels = 4;
        T = GPUTypeFromToGL(0, texstruct, 1);
    end
end

foo = size(T)

while ~KbCheck
    T = T .* 0.99;
    H = T .* (0.5 + 0.5 * sin(GetSecs * 10));
    if 1
        t = GPUTypeFromToGL(1, H, 0, t);
        %     Screen('Close', t);
        %     t = GPUTypeFromToGL(1, H, 0);
        
        Screen('DrawTexture', w, t);
    else
        GPUTypeFromToGL(1, H, 2, w);
    end
    Screen('Flip', w);
end

sca;
