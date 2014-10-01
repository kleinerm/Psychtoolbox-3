function glReplacementCodeuiColor4ubVertex3fSUN( rc, r, g, b, a, x, y, z )

% glReplacementCodeuiColor4ubVertex3fSUN  Interface to OpenGL function glReplacementCodeuiColor4ubVertex3fSUN
%
% usage:  glReplacementCodeuiColor4ubVertex3fSUN( rc, r, g, b, a, x, y, z )
%
% C function:  void glReplacementCodeuiColor4ubVertex3fSUN(GLuint rc, GLubyte r, GLubyte g, GLubyte b, GLubyte a, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuiColor4ubVertex3fSUN', rc, r, g, b, a, x, y, z );

return
