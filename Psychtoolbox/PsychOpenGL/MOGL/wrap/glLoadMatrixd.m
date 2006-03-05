function glLoadMatrixd( m )

% glLoadMatrixd  Interface to OpenGL function glLoadMatrixd
%
% usage:  glLoadMatrixd( m )
%
% C function:  void glLoadMatrixd(const GLdouble* m)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glLoadMatrixd', double(m) );

return
