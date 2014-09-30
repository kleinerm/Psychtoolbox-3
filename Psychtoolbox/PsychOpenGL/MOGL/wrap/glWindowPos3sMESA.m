function glWindowPos3sMESA( x, y, z )

% glWindowPos3sMESA  Interface to OpenGL function glWindowPos3sMESA
%
% usage:  glWindowPos3sMESA( x, y, z )
%
% C function:  void glWindowPos3sMESA(GLshort x, GLshort y, GLshort z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3sMESA', x, y, z );

return
