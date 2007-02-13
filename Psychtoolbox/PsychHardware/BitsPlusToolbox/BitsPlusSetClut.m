function BitsPlusSetClut(windowPtr, clut, doFlip)
%  BitsPlusSetClut(windowPtr, clut, [doFlip])
% 
% Prior to using this routine, Bits++ box must be in
% framebuffer load mode.
%
% If 'doFlip' is set to 1, then BitsPlusSetClut will call flip at the
% end of the function.  By default, this value is 1.
%
% 2/28/03   dhb, ptw  Wrote it.
% 18/4/05   ejw converted it to run with OSX version of Psychtoolbox

if nargin < 2 || nargin > 3
    error('Usage: BitsPlusSetClut(windowPtr, clut, [doFlip])');
end

if nargin == 2
    doFlip = true;
end

% Define screen
whichScreen = max(Screen('Screens'));

% encode the LUT
newClutRow = BitsPlusEncodeClutRow(clut);

% Find out how big the window is.
[screenWidth, screenHeight] = Screen('WindowSize', windowPtr);

% check that the screen width is at least 524 pixels
if screenWidth < 524
    error('window is not big enough to encode the Bits++ CLUT');
end 

rect = [0 0 size(newClutRow,2) 1];
Screen('PutImage', windowPtr, newClutRow, rect, rect);

if doFlip
    Screen(windowPtr, 'Flip');
end
