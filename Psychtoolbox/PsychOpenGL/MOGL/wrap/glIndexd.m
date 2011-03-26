function glIndexd( c )

% glIndexd  Interface to OpenGL function glIndexd
%
% usage:  glIndexd( c )
%
% C function:  void glIndexd(GLdouble c)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glIndexd', c );

return
