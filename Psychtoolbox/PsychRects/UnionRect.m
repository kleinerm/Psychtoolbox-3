function newRect = UnionRect(a,b)
% newRect = UnionRect(a,b)
% 
% Returns the smallest rect that contains the two rects a and b.
%
% a and b can each be single rects or Mx4 rect-arrays. If both are
% rect-arrays, they must be of the same size.
%
% See also PsychRects/Contents.

% 7/10/96 dgp  Wrote it.
% 8/5/96  dgp  check rect size.
% 7/26/15 dcn  vectorized.

if size(a,2)~=4 || size(b,2)~=4
	error('Wrong size rect argument. Usage:  newRect=UnionRect(a,b)');
end
if size(b,1) > size(a,1)
    newRect = repmat(a,size(b,1)./size(a,1),1);
else
    newRect = a;
end
newRect(:,2) = min(a(:,2) ,b(:,2));
newRect(:,4) = max(a(:,4) ,b(:,4));
newRect(:,1) = min(a(:,1) ,b(:,1));
newRect(:,3) = max(a(:,3) ,b(:,3));
