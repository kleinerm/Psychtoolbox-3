function glRasterPos3s( x, y, z )

% glRasterPos3s  Interface to OpenGL function glRasterPos3s
%
% usage:  glRasterPos3s( x, y, z )
%
% C function:  void glRasterPos3s(GLshort x, GLshort y, GLshort z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos3s', x, y, z );

return
