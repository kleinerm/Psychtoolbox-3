function glMultMatrixf( m )

% glMultMatrixf  Interface to OpenGL function glMultMatrixf
%
% usage:  glMultMatrixf( m )
%
% C function:  void glMultMatrixf(const GLfloat* m)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMultMatrixf', single(m) );

return
