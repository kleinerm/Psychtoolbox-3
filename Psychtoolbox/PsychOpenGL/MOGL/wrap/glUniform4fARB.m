function glUniform4fARB( location, v0, v1, v2, v3 )

% glUniform4fARB  Interface to OpenGL function glUniform4fARB
%
% usage:  glUniform4fARB( location, v0, v1, v2, v3 )
%
% C function:  void glUniform4fARB(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glUniform4fARB', location, v0, v1, v2, v3 );

return
