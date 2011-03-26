function glTranslatef( x, y, z )

% glTranslatef  Interface to OpenGL function glTranslatef
%
% usage:  glTranslatef( x, y, z )
%
% C function:  void glTranslatef(GLfloat x, GLfloat y, GLfloat z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTranslatef', x, y, z );

return
