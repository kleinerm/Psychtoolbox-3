function glIndexdv( c )

% glIndexdv  Interface to OpenGL function glIndexdv
%
% usage:  glIndexdv( c )
%
% C function:  void glIndexdv(const GLdouble* c)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glIndexdv', double(c) );

return
