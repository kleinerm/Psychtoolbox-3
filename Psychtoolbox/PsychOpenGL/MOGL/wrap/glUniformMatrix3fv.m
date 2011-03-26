function glUniformMatrix3fv( location, count, transpose, value )

% glUniformMatrix3fv  Interface to OpenGL function glUniformMatrix3fv
%
% usage:  glUniformMatrix3fv( location, count, transpose, value )
%
% C function:  void glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glUniformMatrix3fv', location, count, transpose, single(value) );

return
