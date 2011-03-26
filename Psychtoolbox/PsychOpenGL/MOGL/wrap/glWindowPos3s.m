function glWindowPos3s( x, y, z )

% glWindowPos3s  Interface to OpenGL function glWindowPos3s
%
% usage:  glWindowPos3s( x, y, z )
%
% C function:  void glWindowPos3s(GLshort x, GLshort y, GLshort z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3s', x, y, z );

return
