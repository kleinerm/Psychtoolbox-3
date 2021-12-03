function [width,height] = WindowSize(window)
% [width,height] = WindowSize(window)
%
% Returns a window's width and height.
%
% 26/03/2001 fwc based on rectsize.m

if nargin~=1
	error('Usage:  [width,height] = WindowSize(window)');
end
rect=Screen('Rect', window);
width = rect(RectRight) - rect(RectLeft);
height = rect(RectBottom)-rect(RectTop);
