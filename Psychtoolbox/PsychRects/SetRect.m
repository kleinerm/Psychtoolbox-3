function newRect = SetRect(left,top,right,bottom)
% newRect = SetRect(left,top,right,bottom);
%
% Create a rect with the specified coordinates.
% This is equivalent to:
%	newRect=[left,top,right,bottom];
% Each of the input can be either a scalar or
% an M-element vector. For those inputs that are
% vectors, they must all have the same length.
%
% See also PsychRects/Contents.

% dgp 5/12/96  Make sure the created rect is a column, not a row, vector
% dgp 5/12/96  Use symbolic indices, added usage check.
% dhb 5/13/96  Put created rect back as row vector.
% 7/10/96 dgp  "help PsychRects"
% 2/25/97 dgp  updated
% 7/26/15 dcn  vectorized

if nargin~=4
    error('Usage: newRect=SetRect(left,top,right,bottom)')
end
szs = [length(left) length(top) length(right) length(bottom)];
if all(szs==1)
    newRect=zeros(1,4);
    newRect(2)=top;
    newRect(1)=left;
    newRect(4)=bottom;
    newRect(3)=right;
else
    maxsz = max(szs);
    if ~all(szs==1 | szs==maxsz)
        error('All inputs must either have one elemnt or the same number of elements as the longest input')
    end
    newRect=zeros(maxsz,4);
    newRect(:,2)=top;
    newRect(:,1)=left;
    newRect(:,4)=bottom;
    newRect(:,3)=right;
end