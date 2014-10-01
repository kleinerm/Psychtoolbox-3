function glTexCoord4fVertex4fSUN( s, t, p, q, x, y, z, w )

% glTexCoord4fVertex4fSUN  Interface to OpenGL function glTexCoord4fVertex4fSUN
%
% usage:  glTexCoord4fVertex4fSUN( s, t, p, q, x, y, z, w )
%
% C function:  void glTexCoord4fVertex4fSUN(GLfloat s, GLfloat t, GLfloat p, GLfloat q, GLfloat x, GLfloat y, GLfloat z, GLfloat w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord4fVertex4fSUN', s, t, p, q, x, y, z, w );

return
