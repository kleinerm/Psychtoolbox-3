function glColor4fNormal3fVertex3fSUN( r, g, b, a, nx, ny, nz, x, y, z )

% glColor4fNormal3fVertex3fSUN  Interface to OpenGL function glColor4fNormal3fVertex3fSUN
%
% usage:  glColor4fNormal3fVertex3fSUN( r, g, b, a, nx, ny, nz, x, y, z )
%
% C function:  void glColor4fNormal3fVertex3fSUN(GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=10,
    error('invalid number of arguments');
end

moglcore( 'glColor4fNormal3fVertex3fSUN', r, g, b, a, nx, ny, nz, x, y, z );

return
