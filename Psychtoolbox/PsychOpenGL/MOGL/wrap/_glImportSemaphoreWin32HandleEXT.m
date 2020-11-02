function handle = glImportSemaphoreWin32HandleEXT( semaphore, handleType )

% glImportSemaphoreWin32HandleEXT  Interface to OpenGL function glImportSemaphoreWin32HandleEXT
%
% usage:  handle = glImportSemaphoreWin32HandleEXT( semaphore, handleType )
%
% C function:  void glImportSemaphoreWin32HandleEXT(GLuint semaphore, GLenum handleType, void* handle)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

handle = (0);

moglcore( 'glImportSemaphoreWin32HandleEXT', semaphore, handleType, handle );

return
