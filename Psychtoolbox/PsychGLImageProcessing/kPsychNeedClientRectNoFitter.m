function rval = kPsychNeedClientRectNoFitter
% rval = kPsychNeedClientRectNoFitter
%
% Return a flag that you can pass to the 'imagingmode' parameter of
% Screen('OpenWindow') in order to allow use of the 'clientRect' passed
% into Screen('Openwindow') without actually using the panel fitter to
% process it.
%
% Without this flag, Screen() will automatically enable its builtin
% panelfitter if it detects the presence of a usercode provided
% 'clientRect'. With this flag, all size reporting functions like
% Screen('Rect') and Screen('WindowSize') etc., as well as all builtin
% 2D drawing functions will act as if the onscreen window had a size
% as specified by 'clientRect'. However, the panel-fitter will be skipped,
% so other processing plugins in the imaging pipeline would be responsible
% for doing the job of treating the 'clientRect' specially. The actual
% virtual framebuffer is the full regular window size, just drawing and
% reporting is restricted to 'clientRect'.
%
% The purpose of this flag is to allow to use drawing areas smaller than
% the true framebuffer without incurring the overhead of execution of the
% panel fitter if some other post-processing step will do the necessary
% sampling of the "oversized" input buffer. Main application is currently
% VR head mounted display support, e.g., the Oculus VR Rift HMDs.
%
rval = 2^19;
return
