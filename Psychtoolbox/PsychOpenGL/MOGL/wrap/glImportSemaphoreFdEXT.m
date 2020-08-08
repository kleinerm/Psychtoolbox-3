function glImportSemaphoreFdEXT( semaphore, handleType, fd )

% glImportSemaphoreFdEXT  Interface to OpenGL function glImportSemaphoreFdEXT
%
% usage:  glImportSemaphoreFdEXT( semaphore, handleType, fd )
%
% C function:  void glImportSemaphoreFdEXT(GLuint semaphore, GLenum handleType, GLint fd)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glImportSemaphoreFdEXT', semaphore, handleType, fd );

return
