function glWindowPos3d( x, y, z )

% glWindowPos3d  Interface to OpenGL function glWindowPos3d
%
% usage:  glWindowPos3d( x, y, z )
%
% C function:  void glWindowPos3d(GLdouble x, GLdouble y, GLdouble z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3d', x, y, z );

return
