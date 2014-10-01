function glReplacementCodeuiColor3fVertex3fSUN( rc, r, g, b, x, y, z )

% glReplacementCodeuiColor3fVertex3fSUN  Interface to OpenGL function glReplacementCodeuiColor3fVertex3fSUN
%
% usage:  glReplacementCodeuiColor3fVertex3fSUN( rc, r, g, b, x, y, z )
%
% C function:  void glReplacementCodeuiColor3fVertex3fSUN(GLuint rc, GLfloat r, GLfloat g, GLfloat b, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuiColor3fVertex3fSUN', rc, r, g, b, x, y, z );

return
