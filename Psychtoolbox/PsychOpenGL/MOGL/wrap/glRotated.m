function glRotated( angle, x, y, z )

% glRotated  Interface to OpenGL function glRotated
%
% usage:  glRotated( angle, x, y, z )
%
% C function:  void glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glRotated', angle, x, y, z );

return
