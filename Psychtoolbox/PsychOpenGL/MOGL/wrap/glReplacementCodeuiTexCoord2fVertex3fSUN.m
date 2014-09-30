function glReplacementCodeuiTexCoord2fVertex3fSUN( rc, s, t, x, y, z )

% glReplacementCodeuiTexCoord2fVertex3fSUN  Interface to OpenGL function glReplacementCodeuiTexCoord2fVertex3fSUN
%
% usage:  glReplacementCodeuiTexCoord2fVertex3fSUN( rc, s, t, x, y, z )
%
% C function:  void glReplacementCodeuiTexCoord2fVertex3fSUN(GLuint rc, GLfloat s, GLfloat t, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuiTexCoord2fVertex3fSUN', rc, s, t, x, y, z );

return
