function glClearBufferfv( buffer, drawbuffer, value )

% glClearBufferfv  Interface to OpenGL function glClearBufferfv
%
% usage:  glClearBufferfv( buffer, drawbuffer, value )
%
% C function:  void glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glClearBufferfv', buffer, drawbuffer, single(value) );

return
