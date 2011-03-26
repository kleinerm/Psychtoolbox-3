function glRasterPos4s( x, y, z, w )

% glRasterPos4s  Interface to OpenGL function glRasterPos4s
%
% usage:  glRasterPos4s( x, y, z, w )
%
% C function:  void glRasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos4s', x, y, z, w );

return
