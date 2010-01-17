function r = GrowRect(r,horizontalPixels,verticalPixels)
% r = GrowRect(r,horizontalPixels,verticalPixels)
%
% Grows a rect, by subtracting horizontalPixels from the left coordinate
% and verticalPixels from the top coordinate, and adding horizontalPixels
% to the right coordinate and verticalPixels to the bottom coordinate. Also
% see ScaleRect. Input can be multiple rects, concatenated into a Mx4
% matrix Using a negate number os pixels to grow by would obviously shrink
% the rect, though be careful as no checks are performed to see if the
% resulting rect is valid.
%
% Also see PsychRects.

% 1/13/10 dcn Wrote it, based on ScaleRect

if nargin~=3
	error('Usage: r = GrowRect(r,horizontalPixels,verticalPixels)');
end

if size(r,2)~=4
	error('Wrong size rect argument. Must be a M x 4 matrix.');
end

r = r + repmat(SetRect(-horizontalPixels,-verticalPixels ...
    ,horizontalPixels,verticalPixels),size(r,1),1);
