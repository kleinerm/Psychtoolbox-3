function glUniform4fvARB( location, count, value )

% glUniform4fvARB  Interface to OpenGL function glUniform4fvARB
%
% usage:  glUniform4fvARB( location, count, value )
%
% C function:  void glUniform4fvARB(GLint location, GLsizei count, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform4fvARB', location, count, single(value) );

return
