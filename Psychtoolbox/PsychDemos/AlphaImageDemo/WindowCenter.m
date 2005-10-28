function [x,y] = WindowCenter(window)
% USAGE: [x,y] = WindowCenter(window)
% Returns a window's center point.

% History
% 26/03/2001    fwc made based on rectcenter
% 05-02-04      fwc OS X PTB compatible
if nargin~=1
	error('Usage:  [x,y] = WindowCenter(window)');
end

rect=Screen('Rect',window);
[x,y] = RectCenter(rect);
