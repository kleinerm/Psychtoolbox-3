function glTexCoord4fColor4fNormal3fVertex4fSUN( s, t, p, q, r, g, b, a, nx, ny, nz, x, y, z, w )

% glTexCoord4fColor4fNormal3fVertex4fSUN  Interface to OpenGL function glTexCoord4fColor4fNormal3fVertex4fSUN
%
% usage:  glTexCoord4fColor4fNormal3fVertex4fSUN( s, t, p, q, r, g, b, a, nx, ny, nz, x, y, z, w )
%
% C function:  void glTexCoord4fColor4fNormal3fVertex4fSUN(GLfloat s, GLfloat t, GLfloat p, GLfloat q, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat x, GLfloat y, GLfloat z, GLfloat w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=15,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord4fColor4fNormal3fVertex4fSUN', s, t, p, q, r, g, b, a, nx, ny, nz, x, y, z, w );

return
