function glVertex4d( x, y, z, w )

% glVertex4d  Interface to OpenGL function glVertex4d
%
% usage:  glVertex4d( x, y, z, w )
%
% C function:  void glVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertex4d', x, y, z, w );

return
