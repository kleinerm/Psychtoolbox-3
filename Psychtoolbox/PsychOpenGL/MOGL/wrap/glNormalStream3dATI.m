function glNormalStream3dATI( stream, nx, ny, nz )

% glNormalStream3dATI  Interface to OpenGL function glNormalStream3dATI
%
% usage:  glNormalStream3dATI( stream, nx, ny, nz )
%
% C function:  void glNormalStream3dATI(GLenum stream, GLdouble nx, GLdouble ny, GLdouble nz)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNormalStream3dATI', stream, nx, ny, nz );

return
