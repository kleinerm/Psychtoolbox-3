function glWindowPos3iMESA( x, y, z )

% glWindowPos3iMESA  Interface to OpenGL function glWindowPos3iMESA
%
% usage:  glWindowPos3iMESA( x, y, z )
%
% C function:  void glWindowPos3iMESA(GLint x, GLint y, GLint z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3iMESA', x, y, z );

return
