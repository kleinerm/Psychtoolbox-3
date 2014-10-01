function glWindowPos3fARB( x, y, z )

% glWindowPos3fARB  Interface to OpenGL function glWindowPos3fARB
%
% usage:  glWindowPos3fARB( x, y, z )
%
% C function:  void glWindowPos3fARB(GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3fARB', x, y, z );

return
