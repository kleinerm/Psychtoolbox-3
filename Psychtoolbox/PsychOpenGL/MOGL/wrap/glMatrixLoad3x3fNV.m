function glMatrixLoad3x3fNV( matrixMode, m )

% glMatrixLoad3x3fNV  Interface to OpenGL function glMatrixLoad3x3fNV
%
% usage:  glMatrixLoad3x3fNV( matrixMode, m )
%
% C function:  void glMatrixLoad3x3fNV(GLenum matrixMode, const GLfloat* m)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixLoad3x3fNV', matrixMode, single(m) );

return
