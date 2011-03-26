function glBindFramebufferEXT( target, framebuffer )

% glBindFramebufferEXT  Interface to OpenGL function glBindFramebufferEXT
%
% usage:  glBindFramebufferEXT( target, framebuffer )
%
% C function:  void glBindFramebufferEXT(GLenum target, GLuint framebuffer)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBindFramebufferEXT', target, framebuffer );

return
