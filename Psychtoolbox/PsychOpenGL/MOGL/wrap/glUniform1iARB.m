function glUniform1iARB( location, v0 )

% glUniform1iARB  Interface to OpenGL function glUniform1iARB
%
% usage:  glUniform1iARB( location, v0 )
%
% C function:  void glUniform1iARB(GLint location, GLint v0)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glUniform1iARB', location, v0 );

return
