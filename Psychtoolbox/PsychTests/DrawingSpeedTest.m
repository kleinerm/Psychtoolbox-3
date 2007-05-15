function DrawingSpeedTest(n,primitivetype)
% DrawingSpeedTest([n=800][,primitivetype=0])
%
% Tests batch-drawing performance of some Screen functions. Batch drawing
% is a way to submit multiple primitives, e.g., Filled Rects, at once. This
% way the Matlab call overhead and some internal setup overhead is saved
% and drawing should be significantly faster than when issuing single
% drawing commands in a loop.
%
% This currently only tests filled rects and framed rects. Dots and Lines are nicely
% demonstrated by DotDemo and LinesDemo.
%
% The optional parameter n allows to specifiy the number of primitives to
% draw each frame, default is 800.

% History:
% 04/30/07 mk Wrote it.

if nargin < 1 || isempty(n)
    n = 800;
end

if nargin < 2 || isempty(primitivetype)
    primitivetype = 0;
end

AssertOpenGL;

screenid = max(Screen('Screens'));
[win winrect] = Screen('OpenWindow', screenid, 128);
w=RectWidth(winrect);
h=RectHeight(winrect);
sizeX=80;
sizeY=80;

% Generate a matrix which specs n filled rectangles, with randomized
% position, color and (dot-,line-)size parameter
for j = 1:n
    posx = rand * w;
    posy = rand * h;
    colors(j, 1:3) = [ rand * 255, rand * 255, rand * 255];
    myrect(j, 1:4) = [ posx, posy, posx + rand * sizeX, posy + rand * sizeY];
    sizes(j) = floor(rand * 10)+1;
end

colors = transpose(colors);
myrect = transpose(myrect);

% Initially sync us to retrace, take start time t1:
t1 = Screen('Flip', win);

% Timing loop, 1000 trials:
for i=1:10000
    % Batch draw:
    switch primitivetype
        case 0,
            Screen('FillRect', win, colors, myrect);            
        case 1,
            Screen('FrameRect', win, colors, myrect, sizes);
    end

    % Flip it. Don't clear buffers, don't sync to retrace. We want the raw
    % speed, nothing limited by monitor refresh:
    Screen('Flip', win, 0, 2, 2);
end

% Make sure the GPU is idle:
Screen('DrawingFinished', win, 2, 1);

% Take end time, compute and print the stats:
telapsed = GetSecs - t1;
tavg = telapsed
tperrect = tavg / n

fprintf('Rendered 1000 frames, each with %i rectangles of size %i x %i.\n', n, sizeX,sizeY);
fprintf('Total time %6.6f seconds. Time per rectangle %6.6f msecs.\n', telapsed, tperrect);

%Done.
Screen('CloseAll');
return;
