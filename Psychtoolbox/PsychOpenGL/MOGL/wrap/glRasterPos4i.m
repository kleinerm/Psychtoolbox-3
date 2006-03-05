function glRasterPos4i( x, y, z, w )

% glRasterPos4i  Interface to OpenGL function glRasterPos4i
%
% usage:  glRasterPos4i( x, y, z, w )
%
% C function:  void glRasterPos4i(GLint x, GLint y, GLint z, GLint w)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos4i', x, y, z, w );

return
