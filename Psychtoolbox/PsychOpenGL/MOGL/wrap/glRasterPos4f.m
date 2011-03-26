function glRasterPos4f( x, y, z, w )

% glRasterPos4f  Interface to OpenGL function glRasterPos4f
%
% usage:  glRasterPos4f( x, y, z, w )
%
% C function:  void glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos4f', x, y, z, w );

return
