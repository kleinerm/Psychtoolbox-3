function rval = kPsychNeedRetinaResolution
% rval = kPsychNeedRetinaResolution
%
% Return a flag that you can pass to the 'imagingmode' parameter of
% Screen('OpenWindow') in order to allow driving a HiDPI display, aka
% Retina-Display at its full native resolution.
%
% Without this flag, Screen() will automatically enable its builtin
% panelfitter if it detects the onscreen window being displayed on a Retina
% style display, ie., with a scaled HiDPI display mode. The panelfitter
% will expose only a smaller, lower resolution framebuffer to the user and
% upscale user drawn stimuli to full panel resolution at Screen('Flip').
% This is the same behaviour as what OSX normally exposes on a Retina
% panel, because it doesn't cause headaches with various coordinate
% transforms, e.g., mouse or touch input coordinates to display coordinates
% and vice versa and it puts less load on the GPU, therefore performance
% impact is minimized. The downside of this default behaviour is that one
% can't take full advantage of the displays high resolution, apart from
% anti-aliasing.
%
% By default Screen() will use the low-level compatibility mode via panel
% fitting. If usercode explicitely requests use of the panelfitter via
% PsychImaging() tasks, then those requests override Screen's default
% behaviour.
%
% If usercode doesn't want panelfitting or low-res modes, but the full
% native Retina resolution, it can pass this flag to express its wish.
% Normally the flag is passed by PsychImaging as part of a more convenient
% setup procedure.
%
rval = 2^18;
return
