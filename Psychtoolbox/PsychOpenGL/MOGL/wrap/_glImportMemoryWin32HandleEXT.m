function handle = glImportMemoryWin32HandleEXT( memory, size, handleType )

% glImportMemoryWin32HandleEXT  Interface to OpenGL function glImportMemoryWin32HandleEXT
%
% usage:  handle = glImportMemoryWin32HandleEXT( memory, size, handleType )
%
% C function:  void glImportMemoryWin32HandleEXT(GLuint memory, GLuint64 size, GLenum handleType, void* handle)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

handle = (0);

moglcore( 'glImportMemoryWin32HandleEXT', memory, uint64(size), handleType, handle );

return
