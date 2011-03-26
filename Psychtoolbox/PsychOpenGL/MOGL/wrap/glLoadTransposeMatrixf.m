function glLoadTransposeMatrixf( m )

% glLoadTransposeMatrixf  Interface to OpenGL function glLoadTransposeMatrixf
%
% usage:  glLoadTransposeMatrixf( m )
%
% C function:  void glLoadTransposeMatrixf(const GLfloat* m)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glLoadTransposeMatrixf', single(m) );

return
