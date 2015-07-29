function r = InsetRect(r,dh,dv)
% r = InsetRect(r,dh,dv)
%
% Shrinks the rectangle by pushing in the left and right
% sides, each by the amount dh, and the top and bottom by the amount dv.
% Negative values of dh and dv produce expansion.
% Also see PsychRects.

% 5/21/96 Denis Pelli
% 7/10/96 dgp PsychRects
% 8/5/96 dgp check rect size.

if nargin~=3
	error('Usage:  r = InsetRect(r,dh,dv)');
end
if size(r,2)~=4
	error('Wrong size rect argument. Usage:  r = InsetRect(r,dh,dv)');
end
r = r+SetRect(dh,dv,-dh,-dv);
