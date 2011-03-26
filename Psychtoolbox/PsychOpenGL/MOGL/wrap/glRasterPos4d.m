function glRasterPos4d( x, y, z, w )

% glRasterPos4d  Interface to OpenGL function glRasterPos4d
%
% usage:  glRasterPos4d( x, y, z, w )
%
% C function:  void glRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos4d', x, y, z, w );

return
