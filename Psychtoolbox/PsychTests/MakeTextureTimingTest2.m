function MakeTextureTimingTest2(screenid, width, height, channels, nSamples, preload, specialFlags, precision)
% MakeTextureTimingTest2([screenid=max][,width=1024][,height=1024][,channels=4][,nSamples=100][,preload=1][,specialFlags=0][,precision=0]);
%
% Test creation timing of a texture of specific 'width' x 'height' size with
% 'channels' color channels (1=Luminance, 2=Luminance+Alpha, 3=RGB,
% 4=RGBA). Also measure texture upload speed if 'preload==1' and drawing
% speed if 'preload==2'. Use 'specialFlags' for texture creation, most
% interestingly a value of 4 to use planar texture storage, which can be
% faster under some circumstances. Create textures of 'precision' - 0 = 8
% bit integer, -1 = 8 bit integer, but created from double() input instead
% of uint8 input, 1 = 16 bpc float or 16 bit signed integer as fallback, 2 =
% 32 bpc float. Use 'nSamples' samples to compute mean timing.
%
% All parameters are optional: Defaults are width x height = 1024 x 1024,
% channels = 4 (RGBA), screenid = max id, 100 samples, preload but don't
% draw, standard storage, 8 bit uint input to 8 bit integer precision.
%
% Shows average duration.
%
% Each run creates a new texture via Screen('MakeTexture'), then preloads
% it onto the graphics card to measure that aspect as well, then deletes
% the texture again. This test will give you rather "worst case" or
% upper-bound numbers on texture management time. In many cases, processing
% of textures of similar size will be faster due to all kinds of internal
% optimizations. Your mileage may vary (TM), but it provides a rough
% impression at least.
%
% see also: PsychTests

% History:
% 06/05/08 mk   Wrote it.

AssertOpenGL;

if nargin < 1
    screenid = [];
end

if isempty(screenid)
    screenid = max(Screen('Screens'));
end

if nargin < 2
    width = [];
end

if nargin < 3
    height = [];
end

if isempty(width)
    width = 1024;
end

if isempty(height)
    height = 1024;
end

if nargin < 4
    channels = [];
end

if isempty(channels)
    channels = 4;
end

if nargin < 5
    nSamples = [];
end

if isempty(nSamples)
    nSamples = 100;
end

if nargin < 6
    preload = [];
end

if isempty(preload)
    preload = 1;
end

if nargin < 7 || isempty(specialFlags)
    specialFlags = 0;
end

if nargin < 8 || isempty(precision)
    precision = 0;
end

try
    % Open standard window:
    InitializeMatlabOpenGL(0,0,1);
    w=Screen('OpenWindow', screenid);

    % Create random test pixel matrix:
    img = uint8(rand(height, width, channels) * 255);
    if precision ~= 0 
        img = double(img);
    end
    
    if precision < 1
        precision = 0;
    end
    
    % Preheat: Screen() may need to allocate internal buffers or create
    % shaders - we don't want this one-time setup overhead to spoil the
    % numbers:
    tex = Screen('MakeTexture', w, img, [], specialFlags, precision);
    Screen('Close', tex);
    
    tbase = Screen('Flip', w);

    % Perform nSamples sampling passes:
    for i=1:nSamples
        tex = Screen('MakeTexture', w, img, [], specialFlags, precision);
        if preload == 1
            Screen('PreloadTextures', w, tex);
        end
        
        if preload == 2
            Screen('DrawTexture', w, tex);
            if i == 1
                Screen('Flip', w, [], 2, 1);
            end
        end
        
        Screen('Close', tex);
    end
    
    if preload ~= 2
        % Enforce completion of all GPU operations, take timestamp:
        elapsed = Screen('DrawingFinished', w, 2, 1);
    else
        elapsed = Screen('Flip', w) - tbase;
    end
    
    avgmsecs = (elapsed / nSamples) * 1000;
    
    fprintf('\n\n\nAverage Make -> Upload -> Destroy time for a %i x %i pixels, %i channels texture over %i samples is: %f msecs.\n\n\n', width, height, channels, nSamples, avgmsecs);

    % Only test common case:
    if (channels >= 3) && (specialFlags == 0) && (precision == 0)
        % Ok, same thing again with low-level calls:
        tex = Screen('MakeTexture', w, img);
        [texid textarget] = Screen('GetOpenGLTexture', w, tex);

        Screen('Flip', w);

        glBindTexture(textarget, texid);
        switch channels
            case 4,
                        glTexImage2D(textarget, 0, GL.RGBA8, width, height, 0, GL.BGRA, GL.UNSIGNED_INT_8_8_8_8_REV, img);
            case 3,
                        glTexImage2D(textarget, 0, GL.RGBA8, width, height, 0, GL.RGB, GL.UNSIGNED_BYTE, img);
        end
        
        glBindTexture(textarget, 0);

        % Perform nSamples sampling passes:
        for i=1:nSamples
            glBindTexture(textarget, texid);
            if channels == 4
                glTexSubImage2D(textarget, 0, 0, 0, width, height, GL.BGRA, GL.UNSIGNED_INT_8_8_8_8_REV, img);
            else
                glTexSubImage2D(textarget, 0, 0, 0, width, height, GL.RGB, GL.UNSIGNED_BYTE, img);
            end
            glBindTexture(textarget, 0);
            if preload
                Screen('PreloadTextures', w, tex);
            end
        end

        % Enforce completion of all GPU operations, take timestamp:
        elapsed = Screen('DrawingFinished', w, 2, 1);
        Screen('Close', tex);

        avgmsecs = (elapsed / nSamples) * 1000;

        fprintf('\n\n\nAverage glTexSubImage2D() -> Upload time for a %i x %i pixels, %i channels texture over %i samples is: %f msecs.\n\n\n', width, height, channels, nSamples, avgmsecs);
    end
    
    Screen('CloseAll');
catch 
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end

return;
