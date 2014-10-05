function glUniform3fvARB( location, count, value )

% glUniform3fvARB  Interface to OpenGL function glUniform3fvARB
%
% usage:  glUniform3fvARB( location, count, value )
%
% C function:  void glUniform3fvARB(GLint location, GLsizei count, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform3fvARB', location, count, single(value) );

return
