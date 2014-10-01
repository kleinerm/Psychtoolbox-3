function glNormalStream3sATI( stream, nx, ny, nz )

% glNormalStream3sATI  Interface to OpenGL function glNormalStream3sATI
%
% usage:  glNormalStream3sATI( stream, nx, ny, nz )
%
% C function:  void glNormalStream3sATI(GLenum stream, GLshort nx, GLshort ny, GLshort nz)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNormalStream3sATI', stream, nx, ny, nz );

return
