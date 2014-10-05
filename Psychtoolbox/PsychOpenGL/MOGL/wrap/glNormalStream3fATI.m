function glNormalStream3fATI( stream, nx, ny, nz )

% glNormalStream3fATI  Interface to OpenGL function glNormalStream3fATI
%
% usage:  glNormalStream3fATI( stream, nx, ny, nz )
%
% C function:  void glNormalStream3fATI(GLenum stream, GLfloat nx, GLfloat ny, GLfloat nz)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNormalStream3fATI', stream, nx, ny, nz );

return
