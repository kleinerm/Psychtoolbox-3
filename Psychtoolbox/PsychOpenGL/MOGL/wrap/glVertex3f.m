function glVertex3f( x, y, z )

% glVertex3f  Interface to OpenGL function glVertex3f
%
% usage:  glVertex3f( x, y, z )
%
% C function:  void glVertex3f(GLfloat x, GLfloat y, GLfloat z)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertex3f', x, y, z );

return
