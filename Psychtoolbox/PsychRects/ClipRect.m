function newRect = ClipRect(a,b)
% newRect = ClipRect(a,b)
% 
% Returns the rect that is the intersection of the two rects a and b.
% Returns an empty rect [0 0 0 0] if the two rects are apart.
%
% a and b can each be single rects or Mx4 rect-arrays. If both are
% rect-arrays, they must be of the same size.
%
% See also PsychRects/Contents.

% 7/5/96  dgp  Wrote it.
% 7/6/96  dgp  Return empty matrix [] only if apart.
% 7/10/96 dgp  Return empty rect [0 0 0 0] if apart.
% 7/26/15 dcn  Vectorized.

if nargin~=2
	error('Usage:  rect=ClipRect(a,b)');
end
if size(a,2)~=4 || size(b,2)~=4
	error('Wrong size rect argument. Usage:  rect=ClipRect(a,b)');
end
if size(b,1) > size(a,1)
    newRect=repmat(a,size(b,1)./size(a,1),1);
else
    newRect = a;
end

newRect(:,RectTop)   =max(a(:,RectTop)   ,b(:,RectTop));
newRect(:,RectBottom)=min(a(:,RectBottom),b(:,RectBottom));
newRect(:,RectLeft)  =max(a(:,RectLeft)  ,b(:,RectLeft));
newRect(:,RectRight) =min(a(:,RectRight) ,b(:,RectRight));

qZero = RectWidth(newRect)<0 | RectHeight(newRect)<0;
newRect(qZero,:) = 0;
