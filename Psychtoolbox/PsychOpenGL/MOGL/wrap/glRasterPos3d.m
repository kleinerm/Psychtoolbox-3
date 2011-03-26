function glRasterPos3d( x, y, z )

% glRasterPos3d  Interface to OpenGL function glRasterPos3d
%
% usage:  glRasterPos3d( x, y, z )
%
% C function:  void glRasterPos3d(GLdouble x, GLdouble y, GLdouble z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos3d', x, y, z );

return
