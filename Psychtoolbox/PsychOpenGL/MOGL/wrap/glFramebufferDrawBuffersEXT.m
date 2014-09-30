function glFramebufferDrawBuffersEXT( framebuffer, n, bufs )

% glFramebufferDrawBuffersEXT  Interface to OpenGL function glFramebufferDrawBuffersEXT
%
% usage:  glFramebufferDrawBuffersEXT( framebuffer, n, bufs )
%
% C function:  void glFramebufferDrawBuffersEXT(GLuint framebuffer, GLsizei n, const GLenum* bufs)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glFramebufferDrawBuffersEXT', framebuffer, n, uint32(bufs) );

return
