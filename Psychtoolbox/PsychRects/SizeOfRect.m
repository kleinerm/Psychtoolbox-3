function s=SizeOfRect(rect)
% s=SizeOfRect(rect);
% Accepts a Psychtoolbox rect [left, top, right, bottom] and returns the
% size [rows columns] of a MATLAB array (i.e. image) just big enough to
% hold all the pixels.
% See also RectOfMatrix, RectHeight, RectWidth, PsychRects.

% 7/16/06 dgp Wrote it.

s=[RectHeight(rect) RectWidth(rect)];
