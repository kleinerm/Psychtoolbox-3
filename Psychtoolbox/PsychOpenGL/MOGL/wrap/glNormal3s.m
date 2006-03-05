function glNormal3s( nx, ny, nz )

% glNormal3s  Interface to OpenGL function glNormal3s
%
% usage:  glNormal3s( nx, ny, nz )
%
% C function:  void glNormal3s(GLshort nx, GLshort ny, GLshort nz)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glNormal3s', nx, ny, nz );

return
