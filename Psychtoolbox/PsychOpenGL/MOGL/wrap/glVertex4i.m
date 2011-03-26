function glVertex4i( x, y, z, w )

% glVertex4i  Interface to OpenGL function glVertex4i
%
% usage:  glVertex4i( x, y, z, w )
%
% C function:  void glVertex4i(GLint x, GLint y, GLint z, GLint w)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertex4i', x, y, z, w );

return
