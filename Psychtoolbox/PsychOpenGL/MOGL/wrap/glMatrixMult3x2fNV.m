function glMatrixMult3x2fNV( matrixMode, m )

% glMatrixMult3x2fNV  Interface to OpenGL function glMatrixMult3x2fNV
%
% usage:  glMatrixMult3x2fNV( matrixMode, m )
%
% C function:  void glMatrixMult3x2fNV(GLenum matrixMode, const GLfloat* m)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixMult3x2fNV', matrixMode, single(m) );

return
