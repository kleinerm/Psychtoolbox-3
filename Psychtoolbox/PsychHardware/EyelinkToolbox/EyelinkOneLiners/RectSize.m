function [width,height] = RectSize(rect)
% [width,height] = RectSize(rect)
%
% Returns the rect's width and height.
%
% 10/10/2000 fwc wrote it based on PsychToolbox RectHeight/RectWidth

if nargin~=1
	error('Usage:  [width,height] = RectSize(rect)');
end
if size(rect,2)~=4
	error('Wrong size rect argument. Usage:  [width,height] = RectSize(rect)');
end
width = rect(RectRight) - rect(RectLeft);
height = rect(RectBottom)-rect(RectTop);
