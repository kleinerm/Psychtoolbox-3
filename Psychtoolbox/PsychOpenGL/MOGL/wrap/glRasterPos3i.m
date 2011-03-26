function glRasterPos3i( x, y, z )

% glRasterPos3i  Interface to OpenGL function glRasterPos3i
%
% usage:  glRasterPos3i( x, y, z )
%
% C function:  void glRasterPos3i(GLint x, GLint y, GLint z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos3i', x, y, z );

return
