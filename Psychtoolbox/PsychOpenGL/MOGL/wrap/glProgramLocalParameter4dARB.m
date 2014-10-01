function glProgramLocalParameter4dARB( target, index, x, y, z, w )

% glProgramLocalParameter4dARB  Interface to OpenGL function glProgramLocalParameter4dARB
%
% usage:  glProgramLocalParameter4dARB( target, index, x, y, z, w )
%
% C function:  void glProgramLocalParameter4dARB(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramLocalParameter4dARB', target, index, x, y, z, w );

return
