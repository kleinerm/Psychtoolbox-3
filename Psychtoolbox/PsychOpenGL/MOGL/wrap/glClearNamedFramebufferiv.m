function glClearNamedFramebufferiv( framebuffer, buffer, drawbuffer, value )

% glClearNamedFramebufferiv  Interface to OpenGL function glClearNamedFramebufferiv
%
% usage:  glClearNamedFramebufferiv( framebuffer, buffer, drawbuffer, value )
%
% C function:  void glClearNamedFramebufferiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glClearNamedFramebufferiv', framebuffer, buffer, drawbuffer, int32(value) );

return
