function glUniformMatrix3fvARB( location, count, transpose, value )

% glUniformMatrix3fvARB  Interface to OpenGL function glUniformMatrix3fvARB
%
% usage:  glUniformMatrix3fvARB( location, count, transpose, value )
%
% C function:  void glUniformMatrix3fvARB(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glUniformMatrix3fvARB', location, count, transpose, single(value) );

return
