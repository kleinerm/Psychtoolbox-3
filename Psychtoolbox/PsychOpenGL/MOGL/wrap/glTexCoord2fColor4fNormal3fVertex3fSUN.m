function glTexCoord2fColor4fNormal3fVertex3fSUN( s, t, r, g, b, a, nx, ny, nz, x, y, z )

% glTexCoord2fColor4fNormal3fVertex3fSUN  Interface to OpenGL function glTexCoord2fColor4fNormal3fVertex3fSUN
%
% usage:  glTexCoord2fColor4fNormal3fVertex3fSUN( s, t, r, g, b, a, nx, ny, nz, x, y, z )
%
% C function:  void glTexCoord2fColor4fNormal3fVertex3fSUN(GLfloat s, GLfloat t, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=12,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2fColor4fNormal3fVertex3fSUN', s, t, r, g, b, a, nx, ny, nz, x, y, z );

return
