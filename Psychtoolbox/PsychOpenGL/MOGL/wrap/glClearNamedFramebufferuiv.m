function glClearNamedFramebufferuiv( framebuffer, buffer, drawbuffer, value )

% glClearNamedFramebufferuiv  Interface to OpenGL function glClearNamedFramebufferuiv
%
% usage:  glClearNamedFramebufferuiv( framebuffer, buffer, drawbuffer, value )
%
% C function:  void glClearNamedFramebufferuiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glClearNamedFramebufferuiv', framebuffer, buffer, drawbuffer, uint32(value) );

return
