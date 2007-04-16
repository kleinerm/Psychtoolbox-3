function FloatTexturePrecisionTest(imagingmode)
% 11 Bit ohne sign, 12 Bit mit sign.
AssertOpenGL;

screenid = max(Screen('Screens'));
imagingmode = 0;

% Open window, with or without imaging pipeline:
[win winRect]= Screen('OpenWindow', screenid, 255, [], [], [], [], [], imagingmode);

inimg = zeros(256, 256);
count = -2048 + 1/128000;
inc = 1/65535;

for x=1:256
    for y=1:256
        inimg(y,x) = count * inc;
        count = count + 1;
    end
end

minimum = min(min(inimg))
maximum = max(max(inimg))
imagesc(inimg);

% Build 16 bpc texture:
bpc16tex = Screen('MakeTexture', win, inimg, [], [], 1);

% Build 32 bpc reference texture:
bpc32tex = Screen('MakeTexture', win, inimg, [], [], 2);

% Load difference shader:
diffshader = LoadGLSLProgramFromFiles('ColorCodeDifferenceImageShader', 1);
diffoperator = Screen('OpenProxy', win);
Screen('HookFunction', diffoperator, 'AppendShader', 'UserDefinedBlit', 'Texture difference color coding operator', diffshader);
Screen('HookFunction', diffoperator, 'Enable', 'UserDefinedBlit');
glUseProgram(diffshader);
glUniform1i(glGetUniformLocation(diffshader, 'Image1'), 0);
glUniform1i(glGetUniformLocation(diffshader, 'Image2'), 1);
glUniform1f(glGetUniformLocation(diffshader, 'Delta'), inc/2);
glUseProgram(0);

difftex = Screen('TransformTexture', bpc16tex, diffoperator, bpc32tex);

x=15
Screen('DrawLine', difftex, [255 255 0], x, 0, x, 600);
Screen('DrawTexture', win, difftex, [], winRect, [], 0);

Screen('Flip', win);

while KbCheck; end;
KbWait;
Screen('CloseAll');
