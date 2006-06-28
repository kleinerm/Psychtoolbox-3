function glMultMatrixf( m )

% glMultMatrixf  Interface to OpenGL function glMultMatrixf
%
% usage:  glMultMatrixf( m )
%
% C function:  void glMultMatrixf(const GLfloat* m)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMultMatrixf', moglsingle(m) );

return
