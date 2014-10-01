function glNormalStream3iATI( stream, nx, ny, nz )

% glNormalStream3iATI  Interface to OpenGL function glNormalStream3iATI
%
% usage:  glNormalStream3iATI( stream, nx, ny, nz )
%
% C function:  void glNormalStream3iATI(GLenum stream, GLint nx, GLint ny, GLint nz)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNormalStream3iATI', stream, nx, ny, nz );

return
