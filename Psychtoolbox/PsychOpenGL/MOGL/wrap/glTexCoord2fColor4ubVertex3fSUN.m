function glTexCoord2fColor4ubVertex3fSUN( s, t, r, g, b, a, x, y, z )

% glTexCoord2fColor4ubVertex3fSUN  Interface to OpenGL function glTexCoord2fColor4ubVertex3fSUN
%
% usage:  glTexCoord2fColor4ubVertex3fSUN( s, t, r, g, b, a, x, y, z )
%
% C function:  void glTexCoord2fColor4ubVertex3fSUN(GLfloat s, GLfloat t, GLubyte r, GLubyte g, GLubyte b, GLubyte a, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2fColor4ubVertex3fSUN', s, t, r, g, b, a, x, y, z );

return
