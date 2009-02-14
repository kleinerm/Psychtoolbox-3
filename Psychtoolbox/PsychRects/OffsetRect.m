function newRect = OffsetRect(oldRect,x,y)
% newRect = OffsetRect(oldRect,x,y)
%
% Offset the passed rect matrix by the horizontal (x)
% and vertical (y) shift given.
% Also see PsychRects.

% 5/16/96  dhb  Relented to Pelli's request to change calling order
%               from v,h to x,y.
%          dhb  Index using RectTop etc.
% 7/10/96  dgp  Wrote it.
% 8/5/96   dgp  Check rect size.
% 5/18/08  mk   Vectorized.

if nargin~=3
	error('Usage:  newRect = OffsetRect(oldRect,x,y)');
end

if PsychNumel(oldRect) == 4
    % Single rect case:
    if size(oldRect,2)~=4
        error('Wrong size rect argument. Usage:  newRect = OffsetRect(oldRect,x,y)');
    end
    newRect(RectTop) = oldRect(RectTop) + y;
    newRect(RectBottom) = oldRect(RectBottom) + y;
    newRect(RectLeft) = oldRect(RectLeft) + x;
    newRect(RectRight) = oldRect(RectRight) + x;
else
    % Multirect case:
    if size(oldRect, 1)==4
        newRect(RectTop, :) = oldRect(RectTop, :) + y;
        newRect(RectBottom, :) = oldRect(RectBottom, :) + y;
        newRect(RectLeft, :) = oldRect(RectLeft, :) + x;
        newRect(RectRight, :) = oldRect(RectRight, :) + x;
    else
        newRect(:, RectTop) = oldRect(:, RectTop) + y;
        newRect(:, RectBottom) = oldRect(:, RectBottom) + y;
        newRect(:, RectLeft) = oldRect(:, RectLeft) + x;
        newRect(:, RectRight) = oldRect(:, RectRight) + x;
    end
end
