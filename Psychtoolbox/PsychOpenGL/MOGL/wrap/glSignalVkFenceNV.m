function glSignalVkFenceNV( vkFence )

% glSignalVkFenceNV  Interface to OpenGL function glSignalVkFenceNV
%
% usage:  glSignalVkFenceNV( vkFence )
%
% C function:  void glSignalVkFenceNV(GLuint64 vkFence)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSignalVkFenceNV', uint64(vkFence) );

return
