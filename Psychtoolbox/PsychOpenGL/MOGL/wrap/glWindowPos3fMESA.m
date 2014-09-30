function glWindowPos3fMESA( x, y, z )

% glWindowPos3fMESA  Interface to OpenGL function glWindowPos3fMESA
%
% usage:  glWindowPos3fMESA( x, y, z )
%
% C function:  void glWindowPos3fMESA(GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3fMESA', x, y, z );

return
