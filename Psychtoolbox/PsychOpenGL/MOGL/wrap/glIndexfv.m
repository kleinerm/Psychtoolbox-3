function glIndexfv( c )

% glIndexfv  Interface to OpenGL function glIndexfv
%
% usage:  glIndexfv( c )
%
% C function:  void glIndexfv(const GLfloat* c)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glIndexfv', moglsingle(c) );

return
