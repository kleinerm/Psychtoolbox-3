function glNormal3b( nx, ny, nz )

% glNormal3b  Interface to OpenGL function glNormal3b
%
% usage:  glNormal3b( nx, ny, nz )
%
% C function:  void glNormal3b(GLbyte nx, GLbyte ny, GLbyte nz)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glNormal3b', nx, ny, nz );

return
