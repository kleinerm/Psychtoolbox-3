function glWaitVkSemaphoreNV( vkSemaphore )

% glWaitVkSemaphoreNV  Interface to OpenGL function glWaitVkSemaphoreNV
%
% usage:  glWaitVkSemaphoreNV( vkSemaphore )
%
% C function:  void glWaitVkSemaphoreNV(GLuint64 vkSemaphore)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWaitVkSemaphoreNV', uint64(vkSemaphore) );

return
