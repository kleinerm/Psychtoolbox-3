function rval = kPsychSkipTimestampingForFlipOnce
% rval = kPsychSkipTimestampingForFlipOnce
%
% Return a flag that you can pass as part of the flipFlags for a call to
% Screen('Hookfunction', windowPtr, 'SetOneshotFlipFlags', flipFlags);
%
% This flag will disable waiting for flip completion (= stimulus onset)
% as well as visual stimulus onset timestamping and timing correctness
% checks during execution of the next Screen('Flip') operation.
%

rval = 2^28;
return
