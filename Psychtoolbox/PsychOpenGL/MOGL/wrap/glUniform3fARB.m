function glUniform3fARB( location, v0, v1, v2 )

% glUniform3fARB  Interface to OpenGL function glUniform3fARB
%
% usage:  glUniform3fARB( location, v0, v1, v2 )
%
% C function:  void glUniform3fARB(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glUniform3fARB', location, v0, v1, v2 );

return
