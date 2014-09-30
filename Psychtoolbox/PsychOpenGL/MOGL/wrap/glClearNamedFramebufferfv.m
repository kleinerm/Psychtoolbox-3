function glClearNamedFramebufferfv( framebuffer, buffer, drawbuffer, value )

% glClearNamedFramebufferfv  Interface to OpenGL function glClearNamedFramebufferfv
%
% usage:  glClearNamedFramebufferfv( framebuffer, buffer, drawbuffer, value )
%
% C function:  void glClearNamedFramebufferfv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glClearNamedFramebufferfv', framebuffer, buffer, drawbuffer, single(value) );

return
