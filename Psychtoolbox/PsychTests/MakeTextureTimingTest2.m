function MakeTextureTimingTest2(screenid, width, height, channels)
% MakeTextureTimingTest2([screenid][,width][,height][,channels]);
%
% Test creation timing of a texture of specific 'width' x 'height' size with
% 'channels' color channels (1=Luminance, 2=Luminance+Alpha, 3=RGB,
% 4=RGBA).
%
% All parameters are optional: Defaults are width x height = 1024 x 1024,
% channels = 4 (RGBA), screenid = max id.
%
% Performs 1000 sampling runs, shows average duration.
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

nSamples = 1000;

try
    % Open standard window:
    w=Screen('OpenWindow', screenid);

    % Create random test pixel matrix:
    img = uint8(rand(height, width, channels) * 255);
    
    Screen('Flip', w);
        
    % Perform nSamples sampling passes:
    for i=1:nSamples
        tex = Screen('MakeTexture', w, img);
        Screen('PreloadTextures', w, tex);
        Screen('Close', tex);
    end
    
    % Enforce completion of all GPU operations, take timestamp:
    elapsed = Screen('DrawingFinished', w, 2, 1);

    Screen('CloseAll');
    
    avgmsecs = (elapsed / nSamples) * 1000;
    
    fprintf('\n\n\nAverage Make -> Upload -> Destroy time for a %i x %i pixels, %i channels texture over %i samples is: %f msecs.\n\n\n', width, height, channels, nSamples, avgmsecs);
    
catch 
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end

return;
