function glRectf( x1, y1, x2, y2 )

% glRectf  Interface to OpenGL function glRectf
%
% usage:  glRectf( x1, y1, x2, y2 )
%
% C function:  void glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glRectf', x1, y1, x2, y2 );

return
