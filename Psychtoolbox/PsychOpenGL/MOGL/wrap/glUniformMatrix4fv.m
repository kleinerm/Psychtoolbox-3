function glUniformMatrix4fv( location, count, transpose, value )

% glUniformMatrix4fv  Interface to OpenGL function glUniformMatrix4fv
%
% usage:  glUniformMatrix4fv( location, count, transpose, value )
%
% C function:  void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glUniformMatrix4fv', location, count, transpose, single(value) );

return
