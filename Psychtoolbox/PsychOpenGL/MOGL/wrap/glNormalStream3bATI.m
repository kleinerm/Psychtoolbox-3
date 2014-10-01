function glNormalStream3bATI( stream, nx, ny, nz )

% glNormalStream3bATI  Interface to OpenGL function glNormalStream3bATI
%
% usage:  glNormalStream3bATI( stream, nx, ny, nz )
%
% C function:  void glNormalStream3bATI(GLenum stream, GLbyte nx, GLbyte ny, GLbyte nz)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNormalStream3bATI', stream, nx, ny, nz );

return
