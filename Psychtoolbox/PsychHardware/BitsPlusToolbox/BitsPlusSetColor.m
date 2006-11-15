function BitsPlusSetColor(windowPtr,entry,rgb)
% SetColorBitspp(windowPtr,entry,rgb)
% 
% Set the specified entry of the clut, using the Bits++ box and our
% frame buffer conventions.
% 
% Prior to using this routine, Bits++ box must be in
% framebuffer load mode.
%
% This routine is not required to be speedy, and it isn't.
%
% xx/xx/02  jmh  Wrote it, but didn't comment.
% 2/23/03   dhb  Added comments.
% 2/28/03   dhb, ptw  Add delay to make sure glitch has settled.
%                Changed name, fixed up.
% 3/8/03    dhb  Remove call to bitsplus.

% Get bits rectangle.
bitsPlusRect = [0 0 524 1];

% Open offscreen memory to store old clut
offWindow = SCREEN(windowPtr,'OpenOffscreenWindow',0,bitsPlusRect);

% Extract the current clut and decode
currentClutRow = double(Screen(windowPtr,'GetImage',bitsPlusRect));
currentClut = BitsPlusDecodeClutRow(currentClutRow);

% Set the appropriate entry
newClut = currentClut; newClut(entry+1,:) = rgb';

% Write it into offscreen memory
newClutRow = BitsPlusEncodeClutRow(newClut);
Screen(offWindow,'PutImage',newClutRow,bitsPlusRect);

% Copy it fast and in sync with video blanking
Screen(windowPtr,'WaitBlanking');
WaitSecs(0.003);
Screen('CopyWindow',offWindow,windowPtr,bitsPlusRect,bitsPlusRect);

% Close up offscreen memory
Screen(offWindow,'Close');
