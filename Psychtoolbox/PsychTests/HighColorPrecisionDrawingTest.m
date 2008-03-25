function HighColorPrecisionDrawingTest(testconfig, maxdepth)
%
% Colorclamping (aka high-precision vertex colors vs. standard vertex colors)
% -1 / 0 / 1 = Unclamped high-res via shader / Unclamped high-res / Clamped.
%
% Framebuffer: (aka bit-depth and format of framebuffer)
% 0 / 1 / 2  = 8 bpc fixed, 16bpc float, 32bpc float.
%
% Textures: (aka texture precision)
% 0 / 1 / 2 = 8 bpc fixed, 16bpc float, 32bpc float.
%
% Samplers: (aka texture sampling method)
% 0 / 1 / 2 = Hardware / PTB-Auto / PTB-Shaders.
%
% Filters: (aka texture filtering method)
% 0 / 1     = Nearest-Neighbour / Bilinear filtering.

close all; 

if nargin < 1 || isempty(testconfig)
    % Empty 'testconfig' or missing: Do all tests.
    testconfig = [0 2 2 1 1]
end

if length(testconfig) ~=5
    error('testconfig vector must have 5 elements!');
end

if nargin < 2 || isempty(maxdepth)
    maxdepth = 16;
end

if maxdepth > 24
    maxdepth = 24;
    fprintf('Max bit depths value clamped to 24 bits -- More bits are not meaningful on current hardware...\n');
end

screenid = max(Screen('Screens'));

ColorClamping = testconfig(1)
Framebuffer = testconfig(2)
Textures = testconfig(3)
Samplers = testconfig(4)
Filters = testconfig(5)

oldsync = Screen('Preference', 'SkipSyncTests', 2);
PsychImaging('PrepareConfiguration')
PsychImaging('AddTask', 'General', 'FloatingPoint32Bit');

% Open window with black background color:
[win rect] = PsychImaging('OpenWindow', screenid, 0, [0 0 25 25], [], [], [], []);

% Set color clamping (and precision) for standard 2D draw commands:
Screen('ColorRange', win, 1, ColorClamping);

Screen('Flip', win);

% Generate testvector of all color values to test. We test the full
% intensity range from 0.0 (black) to 1.0 (white) divided into 2^maxdepth steps
% -- the finest reasonable number of steps for any display device.
% We start with value 1 and decrement to 0, because the largest errors are
% expected at high values (due to characteristics of IEEE floating point
% format), so if user exits test early, he will get useable results:
testcolors = 1 - linspace(0,1,2^maxdepth);
drawncolors = zeros(1, length(testcolors));
i=1;

% Test 1: Precision of backbuffer clears during Screen('Flip'):
for tc = testcolors
    [xm ym buttons] = GetMouse;
    if buttons(1)
        break;
    end
    
    % Set clear color: Incidentally this also fills the backbuffer with the
    % cleared color:
    Screen('FillRect', win, tc);
    
    % Overdraw a smallish 20 x 20 patch in top-left corner with something
    % else, just to make sure the 'FillRect' above doesn't bleed through:
    Screen('FillRect', win, rand, [0 0 20 20]);
    
    % Flip the buffers - this will fill the backbuffer with 'clear' color
    % after flip: We don't sync to retrace to speed things up a bit...
    Screen('Flip', win, 0, 0, 2);
    
    % Readback drawbuffer, top-left 10x10 area, with float precision, only
    % the red/luminance channel:
    patch = Screen('GetImage', win, [0 0 10 10], 'drawBuffer', 1, 1);
    
    % Store result:
    drawncolors(i) = patch(5,5);
    i=i+1;
    
    if mod(i, 1000)==0
        fprintf('At %i th testvalue of %i...\n', i, 2^maxdepth);
        beep; drawnow;
    end
end

Screen('Preference', 'SkipSyncTests', oldsync);
Screen('CloseAll');

% Test done.
deltacolors = single(testcolors(1:i-1)) - drawncolors(1:i-1);
minv = min(abs(deltacolors));
maxv = max(abs(deltacolors));
goodbits = floor(abs(log2(maxv))) - 1;
if goodbits < 0
    goodbits = 0;
end

if goodbits <= maxdepth
    fprintf('Clearbuffer test: Mindiff.: %1.17f, Maxdiff.: %1.17f --> Accurate to %i bits out of max tested bitdepths %i.\n', minv, maxv, goodbits, maxdepth);
else
    fprintf('Clearbuffer test: Mindiff.: %1.17f, Maxdiff.: %1.17f --> Accurate to full tested bitdepth range of %i bits.\n', minv, maxv, maxdepth);
end

plot(deltacolors);
