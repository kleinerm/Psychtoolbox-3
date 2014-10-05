function glUniform2fvARB( location, count, value )

% glUniform2fvARB  Interface to OpenGL function glUniform2fvARB
%
% usage:  glUniform2fvARB( location, count, value )
%
% C function:  void glUniform2fvARB(GLint location, GLsizei count, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2fvARB', location, count, single(value) );

return
