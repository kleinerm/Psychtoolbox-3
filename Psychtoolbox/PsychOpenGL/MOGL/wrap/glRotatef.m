function glRotatef( angle, x, y, z )

% glRotatef  Interface to OpenGL function glRotatef
%
% usage:  glRotatef( angle, x, y, z )
%
% C function:  void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glRotatef', angle, x, y, z );

return
