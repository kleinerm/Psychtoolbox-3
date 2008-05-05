function [newX, newY] = bvlDrawText(windowPtr, strMessage, x, y, iMirror, iSize)
% [newX, newY] = bvlDrawText(windowPtr, strMessage, [x,] [y,] [iMirror,]
% [iSize])
%
% Draw text to the screen.
% windowPtr is the window pointer returned from Screen (PsychToolbox)
% strMessage is text message to present, as a string
% x, y specifying the pixel coordinates in screen space.  Default is screen center.
% iMirror can be either 0 or 1, specifies whether to mirror the message so
%   the text will be readable on a stereoscope
%   Default is not mirrored.
% iSize - Size of the text, 8 pt, 16 pt, etc...
%
% 2007-05-11    - cburns - Updated to use the new Psychtoolbox 3.x
%                   Note: Original version was called DrawText.m.  Updated
%                   to bvl prefix to eliminate any namespace conflicts.
%
% Created: Feb 2006 - cburns - Bankslab, UC Berkeley
%

% Copy of Screen text based functions from PTB 3.0.8 for quick reference
% textModes = Screen('TextModes');
% oldCopyMode=Screen('TextMode', windowPtr [,textMode]);
% oldTextSize=Screen('TextSize', windowPtr [,textSize]);
% oldStyle=Screen('TextStyle', windowPtr [,style]);
% [oldFontName,oldFontNumber]=Screen(windowPtr,'TextFont' [,fontNameOrNumber]);
% [normBoundsRect, offsetBoundsRect]=Screen('TextBounds', windowPtr, text);
% [newX,newY]=Screen('DrawText', windowPtr, text [,x] [,y] [,color] [,backgroundColor] [,yPositionIsBaseline]);
% oldTextColor=Screen('TextColor', windowPtr [,colorVector]);
% oldTextBackgroundColor=Screen('TextBackgroundColor', windowPtr [,colorVector]);

% check args and set defaults
if (nargin < 1) || (length(strMessage) < 1)
    return;
end

if (nargin < 6)
    iSize = 24;
end
if (nargin < 5)
    iMirror = 0;
end

[width, height] = Screen('WindowSize', windowPtr);
if (nargin < 4)
    y = height / 2;
end
if (nargin < 3)
    x = width / 2;
end

% Set text properties
if ~IsLinux
    Screen(windowPtr, 'TextFont', 'Times');
end
Screen('TextSize', windowPtr, iSize);

if (iMirror == 1)
    % Portions of this taken from PsychDemos/DrawMirroredTextDemo.m
    
    % Get text bounds
    [textBounds, offsetBounds] = Screen('TextBounds', windowPtr, strMessage);
    % Offset to it's screenspace
    textBounds = OffsetRect(textBounds, x, y);
    % Find center of textRect
    [xCenter, yCenter] = RectCenter(textBounds);
    % Setup a local transform
    Screen('glPushMatrix', windowPtr);

    % Translate origin into the geometric center of text:
    Screen('glTranslate', windowPtr, xCenter, 0, 0);
    % Apple a scaling transform which flips the diretion of x-Axis,
    % thereby mirroring the drawn text horizontally:
    Screen('glScale', windowPtr, -1, 1, 1);
    % We need to undo the translations...
    Screen('glTranslate', windowPtr, -xCenter, 0, 0);
    % The transformation is ready for mirrored drawing of text:
    Screen('DrawText', windowPtr, strMessage, x, y);
    % Pop local transformation of the stack
    Screen('glPopMatrix', windowPtr);
else
    Screen('DrawText', windowPtr, strMessage, x, y);
end
