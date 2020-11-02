function glMatrixMultTranspose3x3fNV( matrixMode, m )

% glMatrixMultTranspose3x3fNV  Interface to OpenGL function glMatrixMultTranspose3x3fNV
%
% usage:  glMatrixMultTranspose3x3fNV( matrixMode, m )
%
% C function:  void glMatrixMultTranspose3x3fNV(GLenum matrixMode, const GLfloat* m)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixMultTranspose3x3fNV', matrixMode, single(m) );

return
