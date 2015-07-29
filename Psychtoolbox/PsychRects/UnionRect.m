function newRect = UnionRect(a,b)
% newRect = UnionRect(a,b)
% 
% Returns the smallest rect that contains the two rects a and b.
% Also see PsychRects.

% 7/10/96 dgp  Wrote it.
% 8/5/96 dgp check rect size.

if size(a,2)~=4 || size(b,2)~=4
	error('Wrong size rect argument. Usage:  newRect=UnionRect(a,b)');
end
newRect=a;
newRect(RectTop)=min(a(RectTop),b(RectTop));
newRect(RectBottom)=max(a(RectBottom),b(RectBottom));
newRect(RectLeft)=min(a(RectLeft),b(RectLeft));
newRect(RectRight)=max(a(RectRight),b(RectRight));
