function rval = kPsychSkipSwapForFlipOnce
% rval = kPsychSkipSwapForFlipOnce
%
% Return a flag that you can pass as part of the flipFlags for a call to
% Screen('Hookfunction', windowPtr, 'SetOneshotFlipFlags', flipFlags);
%
% This flag will disable the actual visual stimulus presentation to
% the onscreen window via OpenGL double-buffer swap during execution
% of the next Screen('Flip') operation, ergo also any timestamping and
% wait for swap completion. It will go through all the moves, post-
% processing the stimulus, computing the final image, and most post-flip
% operations, but the results of this work will not be shown as a visual
% update to the onscreen window.
%

rval = 2^29;
return
