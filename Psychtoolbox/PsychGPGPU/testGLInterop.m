function testGLInterop

Screen('Preference','SkipSyncTests', 2);
w = Screen('OpenWindow', 0, 0);

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
Screen('Flip', w);

T = GPUTypeFromToGL(0, bunnytex);
foo = size(T)

while ~KbCheck
    T = T .* 0.99;
    H = T .* (0.5 + 0.5 * sin(GetSecs * 10));
    t = GPUTypeFromToGL(1, H, 0, t);
%     Screen('Close', t);
%     t = GPUTypeFromToGL(1, H, 0);
    
    Screen('DrawTexture', w, t);
    Screen('Flip', w);
end

sca;
