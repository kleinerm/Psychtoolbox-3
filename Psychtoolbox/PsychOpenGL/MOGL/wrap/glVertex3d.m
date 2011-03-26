function glVertex3d( x, y, z )

% glVertex3d  Interface to OpenGL function glVertex3d
%
% usage:  glVertex3d( x, y, z )
%
% C function:  void glVertex3d(GLdouble x, GLdouble y, GLdouble z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertex3d', x, y, z );

return
