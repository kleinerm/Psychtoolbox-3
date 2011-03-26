function glUniform4fv( location, count, value )

% glUniform4fv  Interface to OpenGL function glUniform4fv
%
% usage:  glUniform4fv( location, count, value )
%
% C function:  void glUniform4fv(GLint location, GLsizei count, const GLfloat* value)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform4fv', location, count, single(value) );

return
