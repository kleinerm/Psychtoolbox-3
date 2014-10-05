function glColor3fVertex3fSUN( r, g, b, x, y, z )

% glColor3fVertex3fSUN  Interface to OpenGL function glColor3fVertex3fSUN
%
% usage:  glColor3fVertex3fSUN( r, g, b, x, y, z )
%
% C function:  void glColor3fVertex3fSUN(GLfloat r, GLfloat g, GLfloat b, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glColor3fVertex3fSUN', r, g, b, x, y, z );

return
