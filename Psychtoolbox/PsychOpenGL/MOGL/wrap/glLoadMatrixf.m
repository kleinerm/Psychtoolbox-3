function glLoadMatrixf( m )

% glLoadMatrixf  Interface to OpenGL function glLoadMatrixf
%
% usage:  glLoadMatrixf( m )
%
% C function:  void glLoadMatrixf(const GLfloat* m)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glLoadMatrixf', single(m) );

return
