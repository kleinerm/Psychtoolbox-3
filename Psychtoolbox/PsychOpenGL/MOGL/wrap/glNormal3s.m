function glNormal3s( nx, ny, nz )

% glNormal3s  Interface to OpenGL function glNormal3s
%
% usage:  glNormal3s( nx, ny, nz )
%
% C function:  void glNormal3s(GLshort nx, GLshort ny, GLshort nz)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glNormal3s', nx, ny, nz );

return
