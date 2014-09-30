function glTexCoord2fVertex3fSUN( s, t, x, y, z )

% glTexCoord2fVertex3fSUN  Interface to OpenGL function glTexCoord2fVertex3fSUN
%
% usage:  glTexCoord2fVertex3fSUN( s, t, x, y, z )
%
% C function:  void glTexCoord2fVertex3fSUN(GLfloat s, GLfloat t, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2fVertex3fSUN', s, t, x, y, z );

return
