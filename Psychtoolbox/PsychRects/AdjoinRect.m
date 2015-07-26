function r=AdjoinRect(r,fixedRect,fixedSide)
% rect=AdjoinRect(rect,fixedRect,fixedSide)
%
% Moves rect to be outside and alongside the specified edge of fixedRect.
% The legal values for fixedSide are: RectLeft, RectRight, 
% RectTop, and RectBottom.
%
% rect and fixedRect can both be Mx4 rect arrays, but either for one of
% them M must be 1, or rect and fixedRect must have the same shape. If rect
% contains multiple rects and fixedRect only one, each rect is moved w.r.t.
% fixedRect. If fixedRect contains multiple rects, but rect only one, rect
% is moved w.r.t each of the fixedRects. If both rect and fixedRect are
% arrays with the same number of rects, each rect is moved w.r.t. the
% corresponding fixedrect. fixedSide should always be scalar: the same
% operation will be executed on all rects in the array.
%
% See also PsychRects/Contents.

% Denis Pelli 5/26/96, 7/10/96, 8/6/96
% dcn 7/26/2015: vectorized, cleaned up

if nargin~=3
	error('Usage: rect=AdjoinRect(rect,fixedRect,fixedSide)');
end
if size(r,2)~=4 || size(fixedRect,2)~=4
	error('Wrong size rect argument. Usage:  rect=AdjoinRect(rect,fixedRect,fixedSide)');
end
if ~isscalar(fixedSide)
	error('Wrong size fixedSide argument, must be scalar. Usage:  rect=AdjoinRect(rect,fixedRect,fixedSide)');
end

switch fixedSide
    case RectLeft
        r=OffsetRect(r,fixedRect(:,fixedSide)-r(:,RectRight),0);
    case RectRight
        r=OffsetRect(r,fixedRect(:,fixedSide)-r(:,RectLeft),0);
    case RectTop
        r=OffsetRect(r,0,fixedRect(:,fixedSide)-r(:,RectBottom));
    case RectBottom
        r=OffsetRect(r,0,fixedRect(:,fixedSide)-r(:,RectTop));
    otherwise
        error('Illegal value %f of ''fixedSide''',fixedSide);
end
