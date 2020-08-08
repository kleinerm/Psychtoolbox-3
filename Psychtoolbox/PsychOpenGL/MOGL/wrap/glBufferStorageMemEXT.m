function glBufferStorageMemEXT( target, size, memory, offset )

% glBufferStorageMemEXT  Interface to OpenGL function glBufferStorageMemEXT
%
% usage:  glBufferStorageMemEXT( target, size, memory, offset )
%
% C function:  void glBufferStorageMemEXT(GLenum target, GLsizeiptr size, GLuint memory, GLuint64 offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBufferStorageMemEXT', target, size, memory, uint64(offset) );

return
