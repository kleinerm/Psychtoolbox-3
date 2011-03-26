function glScalef( x, y, z )

% glScalef  Interface to OpenGL function glScalef
%
% usage:  glScalef( x, y, z )
%
% C function:  void glScalef(GLfloat x, GLfloat y, GLfloat z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glScalef', x, y, z );

return
