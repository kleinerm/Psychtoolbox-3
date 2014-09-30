function glUniform2iARB( location, v0, v1 )

% glUniform2iARB  Interface to OpenGL function glUniform2iARB
%
% usage:  glUniform2iARB( location, v0, v1 )
%
% C function:  void glUniform2iARB(GLint location, GLint v0, GLint v1)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2iARB', location, v0, v1 );

return
