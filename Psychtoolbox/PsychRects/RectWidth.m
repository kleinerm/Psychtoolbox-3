function width = RectWidth(rect)
% width = RectWidth(rect)
%
% Returns the rect's width.
% Also see PsychRects.

% 5/12/96 dgp wrote it.
% 7/10/96 dgp PsychRects

if nargin~=1
	error('Usage:  width = RectWidth(rect)');
end
if size(rect,2)~=4
	error('Wrong size rect argument. Usage:  width = RectWidth(rect)');
end
width = rect(RectRight) - rect(RectLeft);
