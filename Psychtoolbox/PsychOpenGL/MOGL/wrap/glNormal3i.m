function glNormal3i( nx, ny, nz )

% glNormal3i  Interface to OpenGL function glNormal3i
%
% usage:  glNormal3i( nx, ny, nz )
%
% C function:  void glNormal3i(GLint nx, GLint ny, GLint nz)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glNormal3i', nx, ny, nz );

return
