function rval = kPsychSkipVsyncForFlipOnce
% rval = kPsychSkipVsyncForFlipOnce
%
% Return a flag that you can pass as part of the flipFlags for a call to
% Screen('Hookfunction', windowPtr, 'SetOneshotFlipFlags', flipFlags);
%
% This flag will disable synchronization of OpenGL bufferswaps to
% vertical blank ("VBLANK" / "VSYNC") for the next Screen('Flip') operation.
%

rval = 2^27;
return
