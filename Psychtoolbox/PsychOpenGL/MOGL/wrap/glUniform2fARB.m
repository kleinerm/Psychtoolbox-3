function glUniform2fARB( location, v0, v1 )

% glUniform2fARB  Interface to OpenGL function glUniform2fARB
%
% usage:  glUniform2fARB( location, v0, v1 )
%
% C function:  void glUniform2fARB(GLint location, GLfloat v0, GLfloat v1)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2fARB', location, v0, v1 );

return
