function glColor4ubVertex3fSUN( r, g, b, a, x, y, z )

% glColor4ubVertex3fSUN  Interface to OpenGL function glColor4ubVertex3fSUN
%
% usage:  glColor4ubVertex3fSUN( r, g, b, a, x, y, z )
%
% C function:  void glColor4ubVertex3fSUN(GLubyte r, GLubyte g, GLubyte b, GLubyte a, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glColor4ubVertex3fSUN', r, g, b, a, x, y, z );

return
