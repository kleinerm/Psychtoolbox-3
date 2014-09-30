function glReplacementCodeuiVertex3fSUN( rc, x, y, z )

% glReplacementCodeuiVertex3fSUN  Interface to OpenGL function glReplacementCodeuiVertex3fSUN
%
% usage:  glReplacementCodeuiVertex3fSUN( rc, x, y, z )
%
% C function:  void glReplacementCodeuiVertex3fSUN(GLuint rc, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuiVertex3fSUN', rc, x, y, z );

return
