function glNamedFramebufferDrawBuffers( framebuffer, n, bufs )

% glNamedFramebufferDrawBuffers  Interface to OpenGL function glNamedFramebufferDrawBuffers
%
% usage:  glNamedFramebufferDrawBuffers( framebuffer, n, bufs )
%
% C function:  void glNamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, const GLenum* bufs)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferDrawBuffers', framebuffer, n, uint32(bufs) );

return
