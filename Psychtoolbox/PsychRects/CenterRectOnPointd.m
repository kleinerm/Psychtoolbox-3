function newRect = CenterRectOnPointd(rect,x,y)
% newRect = CenterRectOnPointd(rect,x,y)
%
% CenterRectOnPointd offsets the rect to center it around an x and y position.
% The center of the rect is reported by RectCenterd. This is the same as
% CenterRectOnPoint, just that it returns floating point results instead of
% results that are rounded to integral pixel positions.
%
% See also PsychRects, RectCenter, RectCenterd

% 9/13/99      Allen Ingling wrote it
% 10/6/99  dgp With permission from Allen, changed name from "PinRect", which
%              conflicts with Apple's very different PinRect QuickDraw function,
%              to CenterRectOnPoint. Simplified code.
% 5/18/08 mk   Vectorized, ans uses RectCenterd for non-integral results.
% 8/22/19 mk   Resolve ambiguity around rect = 4x4 matrix.

if size(rect,1) == 4 && size(rect,2) == 4
  [cX,cY] = RectCenterd(rect');
  cX = cX';
  cY = cY';
else
  [cX,cY] = RectCenterd(rect);
end

newRect = OffsetRect(rect,x-cX,y-cY);
