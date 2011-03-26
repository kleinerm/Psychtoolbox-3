function glIndexub( c )

% glIndexub  Interface to OpenGL function glIndexub
%
% usage:  glIndexub( c )
%
% C function:  void glIndexub(GLubyte c)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glIndexub', c );

return
