function glIndexMask( mask )

% glIndexMask  Interface to OpenGL function glIndexMask
%
% usage:  glIndexMask( mask )
%
% C function:  void glIndexMask(GLuint mask)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glIndexMask', mask );

return
