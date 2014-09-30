function glWindowPos4iMESA( x, y, z, w )

% glWindowPos4iMESA  Interface to OpenGL function glWindowPos4iMESA
%
% usage:  glWindowPos4iMESA( x, y, z, w )
%
% C function:  void glWindowPos4iMESA(GLint x, GLint y, GLint z, GLint w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos4iMESA', x, y, z, w );

return
