function newRect = SetRect(left,top,right,bottom)
% newRect = SetRect(left,top,right,bottom);
%
% Create a rect with the specified coordinates.
% This is equivalent to:
%	newRect=[left,top,right,bottom];
% Also see PsychRects.

% dgp 5/12/96  Make sure the created rect is a column, not a row, vector
% dgp 5/12/96  Use symbolic indices, added usage check.
% dhb 5/13/96  Put created rect back as row vector.
% 7/10/96 dgp  "help PsychRects"
% 2/25/97 dgp  updated

if nargin~=4
	error('Usage: newRect=SetRect(left,top,right,bottom)')
end
newRect=zeros(1,4);
newRect(RectTop)=top;
newRect(RectLeft)=left;
newRect(RectBottom)=bottom;
newRect(RectRight)=right;



