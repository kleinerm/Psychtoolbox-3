function glNamedFramebufferDrawBuffer( framebuffer, buf )

% glNamedFramebufferDrawBuffer  Interface to OpenGL function glNamedFramebufferDrawBuffer
%
% usage:  glNamedFramebufferDrawBuffer( framebuffer, buf )
%
% C function:  void glNamedFramebufferDrawBuffer(GLuint framebuffer, GLenum buf)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferDrawBuffer', framebuffer, buf );

return
