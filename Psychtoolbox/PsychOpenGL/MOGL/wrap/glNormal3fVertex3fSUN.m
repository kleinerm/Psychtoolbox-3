function glNormal3fVertex3fSUN( nx, ny, nz, x, y, z )

% glNormal3fVertex3fSUN  Interface to OpenGL function glNormal3fVertex3fSUN
%
% usage:  glNormal3fVertex3fSUN( nx, ny, nz, x, y, z )
%
% C function:  void glNormal3fVertex3fSUN(GLfloat nx, GLfloat ny, GLfloat nz, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glNormal3fVertex3fSUN', nx, ny, nz, x, y, z );

return
