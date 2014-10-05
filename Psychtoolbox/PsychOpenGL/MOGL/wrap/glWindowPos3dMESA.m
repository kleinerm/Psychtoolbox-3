function glWindowPos3dMESA( x, y, z )

% glWindowPos3dMESA  Interface to OpenGL function glWindowPos3dMESA
%
% usage:  glWindowPos3dMESA( x, y, z )
%
% C function:  void glWindowPos3dMESA(GLdouble x, GLdouble y, GLdouble z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3dMESA', x, y, z );

return
