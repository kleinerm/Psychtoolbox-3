function memoryObjects = glCreateMemoryObjectsEXT( n )

% glCreateMemoryObjectsEXT  Interface to OpenGL function glCreateMemoryObjectsEXT
%
% usage:  memoryObjects = glCreateMemoryObjectsEXT( n )
%
% C function:  void glCreateMemoryObjectsEXT(GLsizei n, GLuint* memoryObjects)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

memoryObjects = uint32(0);

moglcore( 'glCreateMemoryObjectsEXT', n, memoryObjects );

return
