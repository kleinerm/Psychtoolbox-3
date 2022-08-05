function rval = kPsychDontUseFlipperThread
% rval = kPsychDontUseFlipperThread
%
% This flag can be passed to the optional 'specialFlags' parameter of
% Screen('OpenWindow', ...) or PsychImaging('OpenWindow', ...), and is
% used internally by PsychImaging for some use-cases.
%
% It prevents conventional use of Screen's internal background flipper thread
% and thereby disables all use-cases and functionality that requires the thread,
% e.g., some modes of fine-grained visual stimulus presentation timing via VRR,
% our own homegrown frame-sequential stereo modes (stereoMode 11), and the async
% flip function Screen('AsyncFlipBegin/Check/End').
%
% The flag will be often used for interoperation with external presentation
% backends, e.g., Virtual Reality / Augmented Reality / Mixed Reality runtimes
% like OpenXR for stimulus presentation by a VR compositor. Other applications
% are conceivable, but OpenXR is the initial target application of this flag.
rval = 2^34;
return
