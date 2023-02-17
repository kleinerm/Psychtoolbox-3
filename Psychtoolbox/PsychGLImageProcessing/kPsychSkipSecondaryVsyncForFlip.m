function rval = kPsychSkipSecondaryVsyncForFlip
% rval = kPsychSkipSecondaryVsyncForFlip
%
% This flag can be passed to the optional 'specialFlags' parameter of
% Screen('OpenWindow', ...) or PsychImaging('OpenWindow', ...).
%
% This flag will disable synchronization of OpenGL bufferswaps of a potential
% secondary / slave window to vertical blank ("VBLANK" / "VSYNC"). Such windows
% are used for the right-eye view in stereomode 10 (dual window stereo), special
% dual-window display modes, and for display mirroring/cloning with the PsychImaging
% task 'MirrorDisplayTo2ndOutputHead', which provides a copy of the visual stimulus
% image presented to the subject also to a secondary window / display for the
% experimenter.
%
% Especially for this latter use case of display mirroring to an experimenter
% control monitor, one usually wants to skip vsync and accept degraded image
% quality / tearing, because tearing doesn't matter much for a simple control
% display, but disabling vsync prevents throttling of the main subject stimulus
% display to the potentially lower video refresh rate of a cheaper/lower-end
% experimenter display.

rval = 2^35;
return
