function glLoadMatrixf( m )

% glLoadMatrixf  Interface to OpenGL function glLoadMatrixf
%
% usage:  glLoadMatrixf( m )
%
% C function:  void glLoadMatrixf(const GLfloat* m)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glLoadMatrixf', moglsingle(m) );

return
