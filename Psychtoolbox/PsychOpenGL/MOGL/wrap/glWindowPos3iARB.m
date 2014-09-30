function glWindowPos3iARB( x, y, z )

% glWindowPos3iARB  Interface to OpenGL function glWindowPos3iARB
%
% usage:  glWindowPos3iARB( x, y, z )
%
% C function:  void glWindowPos3iARB(GLint x, GLint y, GLint z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3iARB', x, y, z );

return
