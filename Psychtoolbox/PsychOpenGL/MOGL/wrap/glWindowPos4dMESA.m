function glWindowPos4dMESA( x, y, z, w )

% glWindowPos4dMESA  Interface to OpenGL function glWindowPos4dMESA
%
% usage:  glWindowPos4dMESA( x, y, z, w )
%
% C function:  void glWindowPos4dMESA(GLdouble x, GLdouble y, GLdouble z, GLdouble w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos4dMESA', x, y, z, w );

return
