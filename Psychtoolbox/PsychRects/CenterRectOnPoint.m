function newRect = CenterRectOnPoint(rect,x,y)
% newRect = CenterRectOnPoint(rect,x,y)
% 
% CenterRectOnPoint offsets the rect to center it around an x and y position.
% The center of the rect is reported by RectCenter.
% 
% See also PsychRects, RectCenter

% 9/13/99      Allen Ingling wrote it
% 10/6/99  dgp With permission from Allen, changed name from "PinRect", which 
%            conflicts with Apple's very different PinRect QuickDraw function, 
%            to CenterRectOnPoint. Simplified code.

[cX,cY] = RectCenter(rect);
newRect=OffsetRect(rect,x-cX,y-cY);
