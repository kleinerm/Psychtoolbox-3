function rval = kPsychNeedFinalizedFBOSinks
% rval = kPsychNeedFinalizedFBOSinks
%
% Return a flag that you can pass to the 'imagingmode' parameter of
% Screen('OpenWindow') in order to tell Screen's imaging pipeline that
% the final output image - after all post-processing - should not be
% written to the onscreen windows OpenGL system backbuffer for presentation
% in the onscreen window/on the onscreen windows associated display
% device. Instead it should be written to an OpenGL framebuffer object,
% more specifically to a non-power of two GL_TEXTURE_2D OpenGL texture
% attached to that OpenGL framebuffer object. The resulting OpenGL texture
% can then be used as a source image for further processing or display by
% non-standard display sinks, e.g., by the VR compositor of a Virtual Reality
% head mounted display, or some highly specialized non-standard proprietary
% display hardware.
%
rval = 2^20;
return
