function glWindowPos3sARB( x, y, z )

% glWindowPos3sARB  Interface to OpenGL function glWindowPos3sARB
%
% usage:  glWindowPos3sARB( x, y, z )
%
% C function:  void glWindowPos3sARB(GLshort x, GLshort y, GLshort z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3sARB', x, y, z );

return
