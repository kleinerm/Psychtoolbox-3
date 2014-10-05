function glUniform1fARB( location, v0 )

% glUniform1fARB  Interface to OpenGL function glUniform1fARB
%
% usage:  glUniform1fARB( location, v0 )
%
% C function:  void glUniform1fARB(GLint location, GLfloat v0)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glUniform1fARB', location, v0 );

return
