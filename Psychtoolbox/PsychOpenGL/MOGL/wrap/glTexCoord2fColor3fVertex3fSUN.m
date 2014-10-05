function glTexCoord2fColor3fVertex3fSUN( s, t, r, g, b, x, y, z )

% glTexCoord2fColor3fVertex3fSUN  Interface to OpenGL function glTexCoord2fColor3fVertex3fSUN
%
% usage:  glTexCoord2fColor3fVertex3fSUN( s, t, r, g, b, x, y, z )
%
% C function:  void glTexCoord2fColor3fVertex3fSUN(GLfloat s, GLfloat t, GLfloat r, GLfloat g, GLfloat b, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2fColor3fVertex3fSUN', s, t, r, g, b, x, y, z );

return
