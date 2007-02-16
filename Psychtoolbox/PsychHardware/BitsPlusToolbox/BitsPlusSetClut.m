function BitsPlusSetClut(windowPtr, clut, rect, doFlip)
%  BitsPlusSetClut(windowPtr, clut, [rect], [doFlip])
% 
% Prior to using this routine, Bits++ box must be in
% framebuffer load mode.
%
% 'rect' lets you define the rect that specifies where the magic code is
% written.  For a typical application, this should be left empty.  However,
% for programs that modify the projection matrix via MOGL, you will want to
% change this to draw the magic code in a more cosmetically appealing
% location.
%
% If 'doFlip' is set to 1, then BitsPlusSetClut will call flip at the
% end of the function.  By default, this value is 1.

% 2/28/03   dhb, ptw  Wrote it.
% 18/4/05   ejw converted it to run with OSX version of Psychtoolbox

if nargin < 2 || nargin > 4
    error('Usage: BitsPlusSetClut(windowPtr, clut, [rect], [doFlip])');
end

switch nargin
    case 2
        rect = [];
        doFlip = true;
    case 3
        doFlip = true;
end

% Make sure that 'rect' has valid dimensions.
if ~isempty(rect) && (size(rect, 1) ~= 1 || size(rect, 2) ~= 4)
    error('rect must be a 4 element vector.');
end

% encode the LUT
newClutRow = BitsPlusEncodeClutRow(clut);

% Find out how big the window is.
[screenWidth, screenHeight] = Screen('WindowSize', windowPtr);

% check that the screen width is at least 524 pixels
if screenWidth < 524
    error('Window is not big enough to encode the Bits++ CLUT.');
end 

if isempty(rect)
    rect = [0, 0, size(newClutRow, 2), 1];
end

% Scale the clut to match the color range.
colorRange = Screen('ColorRange', windowPtr);
newClutRow = newClutRow ./ 255 .* colorRange;

Screen('PutImage', windowPtr, newClutRow, rect);

if doFlip
    Screen(windowPtr, 'Flip');
end
