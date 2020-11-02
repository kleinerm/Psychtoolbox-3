function glMatrixLoadTranspose3x3fNV( matrixMode, m )

% glMatrixLoadTranspose3x3fNV  Interface to OpenGL function glMatrixLoadTranspose3x3fNV
%
% usage:  glMatrixLoadTranspose3x3fNV( matrixMode, m )
%
% C function:  void glMatrixLoadTranspose3x3fNV(GLenum matrixMode, const GLfloat* m)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixLoadTranspose3x3fNV', matrixMode, single(m) );

return
