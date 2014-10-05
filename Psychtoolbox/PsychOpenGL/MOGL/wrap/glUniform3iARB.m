function glUniform3iARB( location, v0, v1, v2 )

% glUniform3iARB  Interface to OpenGL function glUniform3iARB
%
% usage:  glUniform3iARB( location, v0, v1, v2 )
%
% C function:  void glUniform3iARB(GLint location, GLint v0, GLint v1, GLint v2)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glUniform3iARB', location, v0, v1, v2 );

return
