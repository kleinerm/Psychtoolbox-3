function glIndexs( c )

% glIndexs  Interface to OpenGL function glIndexs
%
% usage:  glIndexs( c )
%
% C function:  void glIndexs(GLshort c)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glIndexs', c );

return
