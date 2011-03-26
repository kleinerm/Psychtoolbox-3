function glRasterPos3f( x, y, z )

% glRasterPos3f  Interface to OpenGL function glRasterPos3f
%
% usage:  glRasterPos3f( x, y, z )
%
% C function:  void glRasterPos3f(GLfloat x, GLfloat y, GLfloat z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos3f', x, y, z );

return
