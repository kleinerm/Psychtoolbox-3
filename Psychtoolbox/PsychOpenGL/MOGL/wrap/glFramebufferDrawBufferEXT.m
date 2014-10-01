function glFramebufferDrawBufferEXT( framebuffer, mode )

% glFramebufferDrawBufferEXT  Interface to OpenGL function glFramebufferDrawBufferEXT
%
% usage:  glFramebufferDrawBufferEXT( framebuffer, mode )
%
% C function:  void glFramebufferDrawBufferEXT(GLuint framebuffer, GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glFramebufferDrawBufferEXT', framebuffer, mode );

return
