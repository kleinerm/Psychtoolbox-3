function glDeleteMemoryObjectsEXT( n, memoryObjects )

% glDeleteMemoryObjectsEXT  Interface to OpenGL function glDeleteMemoryObjectsEXT
%
% usage:  glDeleteMemoryObjectsEXT( n, memoryObjects )
%
% C function:  void glDeleteMemoryObjectsEXT(GLsizei n, const GLuint* memoryObjects)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteMemoryObjectsEXT', n, uint32(memoryObjects) );

return
