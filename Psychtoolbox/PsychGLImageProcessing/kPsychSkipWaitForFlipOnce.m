function rval = kPsychSkipWaitForFlipOnce
% rval = kPsychSkipWaitForFlipOnce
%
% Return a flag that you can pass as part of the flipFlags for a call to
% Screen('Hookfunction', windowPtr, 'SetOneshotFlipFlags', flipFlags);
%
% This flag will skip the wait until the 'when' deadline during execution
% of the next Screen('Flip', window, when) operation. It is useful if an
% external display output client/provider, e.g., a VR headset, is active
% and attached to an onscreen window, and the visual stimulus onset
% scheduling should be performed by the dedicated display output provider,
% e.g., if our standard scheduling won't do for such a special purpose device.
% The imaging pipeline provides the special variable "IMAGINGPIPE_FLIPTWHEN"
% which can be used when calling Octave/Matlab runtime function from within
% an imaging pipeline processing chain to pass the 'when' time to the runtime
% function, e.g., for scheduling.

rval = 2^30;
return
