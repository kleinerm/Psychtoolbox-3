function glWindowPos3dARB( x, y, z )

% glWindowPos3dARB  Interface to OpenGL function glWindowPos3dARB
%
% usage:  glWindowPos3dARB( x, y, z )
%
% C function:  void glWindowPos3dARB(GLdouble x, GLdouble y, GLdouble z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3dARB', x, y, z );

return
