function rval = kPsychSinkIsMSAACapable
% rval = kPsychSinkIsMSAACapable
%
% Return a flag that you can pass to the 'imagingmode' parameter of
% Screen('OpenWindow') together with the 'kPsychNeedFinalizedFBOSinks'
% flag. If this kPsychSinkIsMSAACapable flag is given then the
% OpenGL framebuffer objects which are created and attached as final
% output image sinks due to the 'kPsychNeedFinalizedFBOSinks' flag will
% be allowed to have GL_TEXTURE_2D_MULTISAMPLE textures as color attachments,
% and thereby as final target of the output images. Iow., the sink can
% handle multisampled textures.
%
rval = 2^22;
return
