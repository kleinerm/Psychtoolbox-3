function glProgramLocalParameter4fARB( target, index, x, y, z, w )

% glProgramLocalParameter4fARB  Interface to OpenGL function glProgramLocalParameter4fARB
%
% usage:  glProgramLocalParameter4fARB( target, index, x, y, z, w )
%
% C function:  void glProgramLocalParameter4fARB(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramLocalParameter4fARB', target, index, x, y, z, w );

return
