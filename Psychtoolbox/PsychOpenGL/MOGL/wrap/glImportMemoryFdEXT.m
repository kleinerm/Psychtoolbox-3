function glImportMemoryFdEXT( memory, size, handleType, fd )

% glImportMemoryFdEXT  Interface to OpenGL function glImportMemoryFdEXT
%
% usage:  glImportMemoryFdEXT( memory, size, handleType, fd )
%
% C function:  void glImportMemoryFdEXT(GLuint memory, GLuint64 size, GLenum handleType, GLint fd)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glImportMemoryFdEXT', memory, uint64(size), handleType, fd );

return
