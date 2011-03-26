function glVertex2f( x, y )

% glVertex2f  Interface to OpenGL function glVertex2f
%
% usage:  glVertex2f( x, y )
%
% C function:  void glVertex2f(GLfloat x, GLfloat y)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertex2f', x, y );

return
