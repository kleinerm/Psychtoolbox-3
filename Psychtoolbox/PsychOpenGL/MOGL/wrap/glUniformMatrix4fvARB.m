function glUniformMatrix4fvARB( location, count, transpose, value )

% glUniformMatrix4fvARB  Interface to OpenGL function glUniformMatrix4fvARB
%
% usage:  glUniformMatrix4fvARB( location, count, transpose, value )
%
% C function:  void glUniformMatrix4fvARB(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glUniformMatrix4fvARB', location, count, transpose, single(value) );

return
