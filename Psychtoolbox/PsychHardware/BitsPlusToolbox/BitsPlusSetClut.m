function BitsPlusSetClut(windowPtr, clut)
%  BitsPlusSetClut(windowPtr,clut)
% 
% Prior to using this routine, Bits++ box must be in
% framebuffer load mode.
%
% 2/28/03   dhb, ptw  Wrote it.
% 18/4/05   ejw converted it to run with OSX version of Psychtoolbox

% Define screen
whichScreen = max(Screen('Screens'));

% encode the LUT
newClutRow = BitsPlusEncodeClutRow(clut);

% find out how big the window is
[screenWidth, screenHeight] = Screen('WindowSize', windowPtr);

% check that the screen width is at least 524 pixels
if screenWidth < 524
    error('window is not big enough to encode the Bits++ CLUT');
end 

rect = [0 0 size(newClutRow,2) 1];
Screen('PutImage', windowPtr, newClutRow, rect, rect);
Screen(windowPtr, 'Flip');
