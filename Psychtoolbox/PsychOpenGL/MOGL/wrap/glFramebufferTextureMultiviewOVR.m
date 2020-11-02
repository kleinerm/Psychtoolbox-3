function glFramebufferTextureMultiviewOVR( target, attachment, texture, level, baseViewIndex, numViews )

% glFramebufferTextureMultiviewOVR  Interface to OpenGL function glFramebufferTextureMultiviewOVR
%
% usage:  glFramebufferTextureMultiviewOVR( target, attachment, texture, level, baseViewIndex, numViews )
%
% C function:  void glFramebufferTextureMultiviewOVR(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint baseViewIndex, GLsizei numViews)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glFramebufferTextureMultiviewOVR', target, attachment, texture, level, baseViewIndex, numViews );

return
