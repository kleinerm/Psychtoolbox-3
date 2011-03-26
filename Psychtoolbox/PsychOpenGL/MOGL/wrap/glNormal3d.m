function glNormal3d( nx, ny, nz )

% glNormal3d  Interface to OpenGL function glNormal3d
%
% usage:  glNormal3d( nx, ny, nz )
%
% C function:  void glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glNormal3d', nx, ny, nz );

return
