function glRasterPos2d( x, y )

% glRasterPos2d  Interface to OpenGL function glRasterPos2d
%
% usage:  glRasterPos2d( x, y )
%
% C function:  void glRasterPos2d(GLdouble x, GLdouble y)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos2d', x, y );

return
