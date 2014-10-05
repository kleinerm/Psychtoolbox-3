function glUniform4ivARB( location, count, value )

% glUniform4ivARB  Interface to OpenGL function glUniform4ivARB
%
% usage:  glUniform4ivARB( location, count, value )
%
% C function:  void glUniform4ivARB(GLint location, GLsizei count, const GLint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform4ivARB', location, count, int32(value) );

return
