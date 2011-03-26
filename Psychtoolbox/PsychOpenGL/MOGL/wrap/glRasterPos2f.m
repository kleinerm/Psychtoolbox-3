function glRasterPos2f( x, y )

% glRasterPos2f  Interface to OpenGL function glRasterPos2f
%
% usage:  glRasterPos2f( x, y )
%
% C function:  void glRasterPos2f(GLfloat x, GLfloat y)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos2f', x, y );

return
