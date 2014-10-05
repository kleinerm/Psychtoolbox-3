function glWindowPos4fMESA( x, y, z, w )

% glWindowPos4fMESA  Interface to OpenGL function glWindowPos4fMESA
%
% usage:  glWindowPos4fMESA( x, y, z, w )
%
% C function:  void glWindowPos4fMESA(GLfloat x, GLfloat y, GLfloat z, GLfloat w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos4fMESA', x, y, z, w );

return
