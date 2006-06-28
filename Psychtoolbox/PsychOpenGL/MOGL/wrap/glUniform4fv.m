function glUniform4fv( location, count, value )

% glUniform4fv  Interface to OpenGL function glUniform4fv
%
% usage:  glUniform4fv( location, count, value )
%
% C function:  void glUniform4fv(GLint location, GLsizei count, const GLfloat* value)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform4fv', location, count, moglsingle(value) );

return
