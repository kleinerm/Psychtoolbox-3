function glLoadTransposeMatrixd( m )

% glLoadTransposeMatrixd  Interface to OpenGL function glLoadTransposeMatrixd
%
% usage:  glLoadTransposeMatrixd( m )
%
% C function:  void glLoadTransposeMatrixd(const GLdouble* m)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glLoadTransposeMatrixd', double(m) );

return
