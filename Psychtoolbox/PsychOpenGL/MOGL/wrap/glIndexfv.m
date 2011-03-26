function glIndexfv( c )

% glIndexfv  Interface to OpenGL function glIndexfv
%
% usage:  glIndexfv( c )
%
% C function:  void glIndexfv(const GLfloat* c)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glIndexfv', single(c) );

return
