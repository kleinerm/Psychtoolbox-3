function bvlDrawCrosshair(windowPtr, x, y, size, color)
% bvlDrawCrosshair(windowPtr, x, y [,size] [,color])
%
% Draw a crosshair centered at location x,y.
%   windowPtr is the window pointer returned from Screen (PsychToolbox)
%
%   size optionally specifies the size in pixels [default is 32]
%
%   color is a 3x1 color vector - [r g b] with color range from 0:255
%   if color is not specified, rect is drawn in white
%
% Create: 2007-05-15 - cburns

if nargin < 3
    disp('Usage: bvlDrawCrosshair(windowPtr, x, y [,size] [,color])');
    return;
end

if nargin < 4
    size = 32;
end

if nargin < 5
    color = [255 255 255];
end

x0 = x - size/2;
x1 = x + size/2;
y0 = y - size/2;
y1 = y + size/2;
% Horizontal line
Screen('DrawLine', windowPtr, color, x0, y, x1, y, 2);
% Vertical Line
Screen('DrawLine', windowPtr, color, x, y0, x, y1, 2);
