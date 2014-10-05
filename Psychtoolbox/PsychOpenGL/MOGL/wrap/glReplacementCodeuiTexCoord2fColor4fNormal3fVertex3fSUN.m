function glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN( rc, s, t, r, g, b, a, nx, ny, nz, x, y, z )

% glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN  Interface to OpenGL function glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN
%
% usage:  glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN( rc, s, t, r, g, b, a, nx, ny, nz, x, y, z )
%
% C function:  void glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN(GLuint rc, GLfloat s, GLfloat t, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=13,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN', rc, s, t, r, g, b, a, nx, ny, nz, x, y, z );

return
