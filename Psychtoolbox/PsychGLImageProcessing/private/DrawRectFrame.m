function DrawRectFrame(windowPtr, rect, color)
% DrawRectFrame(windowPtr, rect, [color])
%
% Draw a rectangle frame (not filled in) specified by the rect coords.
%   windowPtr is the window pointer returned from Screen (PsychToolbox)
%
%   rect is a 1x4 vector - [x0 y0 x1 y1]
%
%   color is a 1x3 color vector - [r g b] with color range from 0:255
%   if color is not specified, rect is drawn in white
%
% The idea for this came from the Screen('FrameRect') function.  However,
% Screen would only draw from top-left to bottom-right.  This didn't work
% well on the Haploscope when your view is mirrored.
% Draw our own using the Screen('DrawLine') functions.
% 
% REQUIRES:     PsychToolbox version 3.x
%
% Updated:  2007-05-14 - Updated for Psychtoolbox 3.x, using OpenGL.
% Created:  Feb 2006 - cburns - Bankslab, UC Berkeley

if (nargin < 2)
    disp('DrawRectFrame requires at least 2 arguments');
    disp('Usage: DrawRectFrame(windowPtr, rect, [color])');
    return;
end

% Check for color arg
if (nargin == 2)
    color = [255 255 255];
end

% Grab coords
x0 = rect(1);
y0 = rect(2);
x1 = rect(3);
y1 = rect(4);

% Draw lines
Screen('DrawLine', windowPtr, color, x0, y0, x0, y1);
Screen('DrawLine', windowPtr, color, x0, y1, x1, y1);
Screen('DrawLine', windowPtr, color, x1, y1, x1, y0);
Screen('DrawLine', windowPtr, color, x1, y0, x0, y0);
