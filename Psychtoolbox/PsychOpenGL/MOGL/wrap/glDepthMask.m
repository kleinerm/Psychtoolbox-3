function glDepthMask( flag )

% glDepthMask  Interface to OpenGL function glDepthMask
%
% usage:  glDepthMask( flag )
%
% C function:  void glDepthMask(GLboolean flag)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glDepthMask', flag );

return
