function glImportSemaphoreWin32NameEXT( semaphore, handleType, name )

% glImportSemaphoreWin32NameEXT  Interface to OpenGL function glImportSemaphoreWin32NameEXT
%
% usage:  glImportSemaphoreWin32NameEXT( semaphore, handleType, name )
%
% C function:  void glImportSemaphoreWin32NameEXT(GLuint semaphore, GLenum handleType, const void* name)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glImportSemaphoreWin32NameEXT', semaphore, handleType, name );

return
