function glWindowPos4sMESA( x, y, z, w )

% glWindowPos4sMESA  Interface to OpenGL function glWindowPos4sMESA
%
% usage:  glWindowPos4sMESA( x, y, z, w )
%
% C function:  void glWindowPos4sMESA(GLshort x, GLshort y, GLshort z, GLshort w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos4sMESA', x, y, z, w );

return
