function glVertex3s( x, y, z )

% glVertex3s  Interface to OpenGL function glVertex3s
%
% usage:  glVertex3s( x, y, z )
%
% C function:  void glVertex3s(GLshort x, GLshort y, GLshort z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertex3s', x, y, z );

return
