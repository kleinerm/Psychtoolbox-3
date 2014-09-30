function glReplacementCodeuiNormal3fVertex3fSUN( rc, nx, ny, nz, x, y, z )

% glReplacementCodeuiNormal3fVertex3fSUN  Interface to OpenGL function glReplacementCodeuiNormal3fVertex3fSUN
%
% usage:  glReplacementCodeuiNormal3fVertex3fSUN( rc, nx, ny, nz, x, y, z )
%
% C function:  void glReplacementCodeuiNormal3fVertex3fSUN(GLuint rc, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuiNormal3fVertex3fSUN', rc, nx, ny, nz, x, y, z );

return
