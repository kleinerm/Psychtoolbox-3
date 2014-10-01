function glFramebufferReadBufferEXT( framebuffer, mode )

% glFramebufferReadBufferEXT  Interface to OpenGL function glFramebufferReadBufferEXT
%
% usage:  glFramebufferReadBufferEXT( framebuffer, mode )
%
% C function:  void glFramebufferReadBufferEXT(GLuint framebuffer, GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glFramebufferReadBufferEXT', framebuffer, mode );

return
