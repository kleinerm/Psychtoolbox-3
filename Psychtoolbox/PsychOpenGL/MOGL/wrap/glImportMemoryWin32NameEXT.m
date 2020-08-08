function glImportMemoryWin32NameEXT( memory, size, handleType, name )

% glImportMemoryWin32NameEXT  Interface to OpenGL function glImportMemoryWin32NameEXT
%
% usage:  glImportMemoryWin32NameEXT( memory, size, handleType, name )
%
% C function:  void glImportMemoryWin32NameEXT(GLuint memory, GLuint64 size, GLenum handleType, const void* name)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glImportMemoryWin32NameEXT', memory, uint64(size), handleType, name );

return
