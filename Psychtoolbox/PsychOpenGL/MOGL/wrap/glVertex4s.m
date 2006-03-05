function glVertex4s( x, y, z, w )

% glVertex4s  Interface to OpenGL function glVertex4s
%
% usage:  glVertex4s( x, y, z, w )
%
% C function:  void glVertex4s(GLshort x, GLshort y, GLshort z, GLshort w)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertex4s', x, y, z, w );

return
