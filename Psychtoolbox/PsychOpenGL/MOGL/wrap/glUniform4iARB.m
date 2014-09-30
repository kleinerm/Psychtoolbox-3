function glUniform4iARB( location, v0, v1, v2, v3 )

% glUniform4iARB  Interface to OpenGL function glUniform4iARB
%
% usage:  glUniform4iARB( location, v0, v1, v2, v3 )
%
% C function:  void glUniform4iARB(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glUniform4iARB', location, v0, v1, v2, v3 );

return
