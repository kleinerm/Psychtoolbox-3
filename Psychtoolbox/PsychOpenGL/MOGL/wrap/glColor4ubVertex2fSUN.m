function glColor4ubVertex2fSUN( r, g, b, a, x, y )

% glColor4ubVertex2fSUN  Interface to OpenGL function glColor4ubVertex2fSUN
%
% usage:  glColor4ubVertex2fSUN( r, g, b, a, x, y )
%
% C function:  void glColor4ubVertex2fSUN(GLubyte r, GLubyte g, GLubyte b, GLubyte a, GLfloat x, GLfloat y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glColor4ubVertex2fSUN', r, g, b, a, x, y );

return
