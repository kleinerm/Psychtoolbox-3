function [rect,dh,dv] = CenterRect(rect,fixedRect)
% [rect,dh,dv] = CenterRect(rect,fixedRect)
% 
% Center the first rect in the second by adding an integer offset.
% rect and fixedRect can both be Mx4 rect arrays, but either for one of
% them M must be 1, or rect and fixedRect must have the same shape. If rect
% contains multiple rects and fixedRect only one, each rect is centered in
% fixedRect. If fixedRect contains multiple rects, but rect only one, rect
% is centered in each of the fixedRects. If both rect and fixedRect are
% arrays with the same number of rects, each rect is centered in the
% corresponding fixedrect
%
% See also PsychRects/Contents.

% 5/16/96 dgp  Updated for new OffsetRect, and use symbolic rect indices.
% 7/10/96 dgp  PsychRects
% 8/5/96  dgp  Renamed the arguments, check rect size.
% 7/23/97 dgp  Round the offset.
% 5/4/00  dhb  Return dh and dv as well as centered rect.
% 7/26/15 dcn  Vectorized

if nargin~=2
	error('Usage:  rect=CenterRect(rect,fixedRect)');
end
if size(rect,2)~=4 || size(fixedRect,2)~=4
	error('Wrong size rect argument. Usage:  [rect,dh,dv] = CenterRect(rect,fixedRect)');
end
dv=(fixedRect(:,RectTop)+fixedRect(:,RectBottom)-rect(:,RectTop)-rect(:,RectBottom))/2;
dh=(fixedRect(:,RectLeft)+fixedRect(:,RectRight)-rect(:,RectLeft)-rect(:,RectRight))/2;
dv=round(dv);
dh=round(dh);
rect=OffsetRect(rect,dh,dv);


