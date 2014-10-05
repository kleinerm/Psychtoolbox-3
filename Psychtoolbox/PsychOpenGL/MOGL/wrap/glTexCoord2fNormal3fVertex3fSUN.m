function glTexCoord2fNormal3fVertex3fSUN( s, t, nx, ny, nz, x, y, z )

% glTexCoord2fNormal3fVertex3fSUN  Interface to OpenGL function glTexCoord2fNormal3fVertex3fSUN
%
% usage:  glTexCoord2fNormal3fVertex3fSUN( s, t, nx, ny, nz, x, y, z )
%
% C function:  void glTexCoord2fNormal3fVertex3fSUN(GLfloat s, GLfloat t, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2fNormal3fVertex3fSUN', s, t, nx, ny, nz, x, y, z );

return
