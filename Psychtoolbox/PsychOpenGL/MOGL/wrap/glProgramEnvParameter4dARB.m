function glProgramEnvParameter4dARB( target, index, x, y, z, w )

% glProgramEnvParameter4dARB  Interface to OpenGL function glProgramEnvParameter4dARB
%
% usage:  glProgramEnvParameter4dARB( target, index, x, y, z, w )
%
% C function:  void glProgramEnvParameter4dARB(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramEnvParameter4dARB', target, index, x, y, z, w );

return
