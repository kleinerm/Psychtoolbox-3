function inside = IsInRect(x,y,rect)
% inside = IsInRect(x,y,rect)
%
% Is location x,y inside the rect?
% x and/or y can also be column vectors, and rect can be an Mx4 rect-array.
% This means you can test one point against mutliple rects, multiple points
% against one rect or multiple points each against their own rect
%
% See also PsychRects/Contents.

% 3/5/97  dhb  Wrote it.
% 7/26/15 dcn  Vectorized

inside = x >= rect(:,RectLeft) & x <= rect(:,RectRight) & ...
		 y >= rect(:,RectTop)  & y <= rect(:,RectBottom);
