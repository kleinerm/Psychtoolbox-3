function glUniformMatrix2fvARB( location, count, transpose, value )

% glUniformMatrix2fvARB  Interface to OpenGL function glUniformMatrix2fvARB
%
% usage:  glUniformMatrix2fvARB( location, count, transpose, value )
%
% C function:  void glUniformMatrix2fvARB(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glUniformMatrix2fvARB', location, count, transpose, single(value) );

return
