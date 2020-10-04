function rval = kPsychDontAutoResetOneshotFlags
% rval = kPsychDontAutoResetOneshotFlags
%
% Return a flag that you can pass as part of the flipFlags for a call to
% Screen('Hookfunction', windowPtr, 'SetOneshotFlipFlags', flipFlags);
%
% This flag will prevent the one-shot flip flags from being automatically cleared
% after execution of the next Screen('Flip') operation. E.g., flags that won't
% get cleared after Flip are kPsychSkipWaitForFlipOnce, kPsychSkipSwapForFlipOnce,
% kPsychSkipTimestampingForFlipOnce, kPsychSkipVsyncForFlipOnce.
%

rval = 2^33;
return
