function glProgramEnvParameter4fARB( target, index, x, y, z, w )

% glProgramEnvParameter4fARB  Interface to OpenGL function glProgramEnvParameter4fARB
%
% usage:  glProgramEnvParameter4fARB( target, index, x, y, z, w )
%
% C function:  void glProgramEnvParameter4fARB(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramEnvParameter4fARB', target, index, x, y, z, w );

return
