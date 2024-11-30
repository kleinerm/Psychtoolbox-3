function rval = kPsychBackendDecisionMade
% rval = kPsychBackendDecisionMade
%
% This flag can be passed to the optional 'specialFlags' parameter of
% Screen('OpenWindow', ...) or PsychImaging('OpenWindow', ...).
%
% This flag tells Screen('OpenWindow', ...) that a higher level calling
% function of Screen('OpenWindow', ...), e.g., PsychImaging('OpenWindow', ...),
% has made an appropriate, explicit, and intentional choice of the display
% backend that Screen() should use for visual stimulus presentation for the
% given onscreen window, and performed / performs all needed setup, so
% Screen() should not take any initiative on its own, but simply obey
% whatever the calling code has decided for it. Absence of the flag is an
% indicator that Screen('OpenWindow', ...) is called by user written
% experiment scripts directly, which are unaware of specific display
% backend requirements on a given setup, so Screen() may have to make
% backend selection decisions on its own.
%
% In practice, as of November 2024, this flag matters for Screen() running
% on top of macOS on a Apple Silicon Mac with Apples own proprietary gpu
% and display engine on top of Apples proprietary OpenGL implementation,
% which requires use of the Vulkan display backend in most cases for proper
% visual stimulus presentation timing and timestamping. If Screen() detects
% it is running on macOS for Apple Silicon and the flag is present, then it
% knows that high-level code like PsychImaging('OpenWindow', ...) has taken
% care of proper setup. Absence of the flag signals that Screen was likely
% called directly by a users legacy experiment script that was unaware of
% the special requirements of the macOS + Apple Silicon platform. In this
% case, Screen('Openwindow') will recursively call PsychImaging('OpenWindow')
% so PsychImaging can then do the needed appropriate backend decisions and
% itself call back into Screen() with the flag set, to perform actual
% proper configuration. It is essentially a backwards compatibility measure
% to make old legacy scripts work transparently on this special snowflake
% platform.

rval = 2^36;
return
