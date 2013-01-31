function testGLInterop

Screen('Preference','SkipSyncTests', 2);
w = Screen('OpenWindow', 0, 0);

tex = ones(200, 200, 4);
t = Screen('MakeTexture', w, tex, [], [], 2);

% Read our beloved bunny image from filesystem:
bunnyimg = imread([PsychtoolboxRoot 'PsychDemos/konijntjes1024x768.jpg']);
bunnyimg(:,:,4) = 255;
bunnytex = Screen('MakeTexture', w, double(bunnyimg)/255, [], [], 2);
t = bunnytex;

Screen('DrawTexture', w, t);
Screen('Flip', w);

%G = GPUTypeFromGL(w, t);
T = GPUTypeFromToGL(w, bunnytex);
foo = size(T)

while ~KbCheck
    T = T .* 0.99;
    H = T .* (0.5 + 0.5 * sin(GetSecs * 10));
    GPUTypeFromToGL(w, t, H, 1);
    Screen('DrawTexture', w, t);
    Screen('Flip', w);
end

sca;
