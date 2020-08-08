function glMatrixLoad3x2fNV( matrixMode, m )

% glMatrixLoad3x2fNV  Interface to OpenGL function glMatrixLoad3x2fNV
%
% usage:  glMatrixLoad3x2fNV( matrixMode, m )
%
% C function:  void glMatrixLoad3x2fNV(GLenum matrixMode, const GLfloat* m)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixLoad3x2fNV', matrixMode, single(m) );

return
