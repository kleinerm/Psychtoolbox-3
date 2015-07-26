function r = InsetRect(r,dh,dv)
% r = InsetRect(r,dh,dv)
%
% Shrinks the rectangle by pushing in the left and right
% sides, each by the amount dh, and the top and bottom by the amount dv.
% Negative values of dh and dv produce expansion.
% Any or all of the arguments can be rect array to, for instance perform
% the same operation of a number of rects, or shrink/expand by a different
% number of pixels for the one input rect, or multipe input rects
%
% See also PsychRects/Contents.

% 5/21/96 Denis Pelli
% 7/10/96 dgp PsychRects
% 8/5/96  dgp check rect size.
% 7/26/15 dcn vectorized

if nargin~=3
	error('Usage:  r = InsetRect(r,dh,dv)');
end
if size(r,2)~=4
	error('Wrong size rect argument. Usage:  r = InsetRect(r,dh,dv)');
end
dh = dh(:);
dv = dv(:);
if size(r,1)==size(dh,1) && size(r,1)==size(dv,1)
    r = r+SetRect(dh,dv,-dh,-dv);
else
    szs = [size(r,1) size(dh,1) size(dv,1)];
    maxsz = max(szs);
    if ~all(szs==1 | szs==maxsz)
        error('All input must either have one row or the same number of rows as the longest input')
    end
    dh = repmat(dh,maxsz/szs(2),1);
    dv = repmat(dv,maxsz/szs(3),1);
    r  = repmat(r,maxsz/szs(1),1)+SetRect(dh,dv,-dh,-dv);
end
