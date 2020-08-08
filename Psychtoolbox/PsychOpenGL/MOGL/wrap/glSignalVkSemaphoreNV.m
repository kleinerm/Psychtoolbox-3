function glSignalVkSemaphoreNV( vkSemaphore )

% glSignalVkSemaphoreNV  Interface to OpenGL function glSignalVkSemaphoreNV
%
% usage:  glSignalVkSemaphoreNV( vkSemaphore )
%
% C function:  void glSignalVkSemaphoreNV(GLuint64 vkSemaphore)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSignalVkSemaphoreNV', uint64(vkSemaphore) );

return
