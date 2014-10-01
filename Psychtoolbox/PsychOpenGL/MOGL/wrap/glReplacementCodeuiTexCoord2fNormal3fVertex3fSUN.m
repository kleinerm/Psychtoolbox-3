function glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN( rc, s, t, nx, ny, nz, x, y, z )

% glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN  Interface to OpenGL function glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN
%
% usage:  glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN( rc, s, t, nx, ny, nz, x, y, z )
%
% C function:  void glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN(GLuint rc, GLfloat s, GLfloat t, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN', rc, s, t, nx, ny, nz, x, y, z );

return
