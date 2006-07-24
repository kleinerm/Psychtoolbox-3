function [x,y] = windowcenter(window)
% [x,y] = windowcenter(window)
%
% Returns a window's center point.
%
% 26/03/2001 fwc made using on rectcenter

if nargin~=1
	error('Usage:  [x,y] = windowcenter(window)');
end
rect=Screen(window,'Rect');
[x,y] = RectCenter(rect);
