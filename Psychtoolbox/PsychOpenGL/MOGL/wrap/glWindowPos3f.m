function glWindowPos3f( x, y, z )

% glWindowPos3f  Interface to OpenGL function glWindowPos3f
%
% usage:  glWindowPos3f( x, y, z )
%
% C function:  void glWindowPos3f(GLfloat x, GLfloat y, GLfloat z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3f', x, y, z );

return
