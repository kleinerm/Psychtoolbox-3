function glNamedBufferStorageMemEXT( buffer, size, memory, offset )

% glNamedBufferStorageMemEXT  Interface to OpenGL function glNamedBufferStorageMemEXT
%
% usage:  glNamedBufferStorageMemEXT( buffer, size, memory, offset )
%
% C function:  void glNamedBufferStorageMemEXT(GLuint buffer, GLsizeiptr size, GLuint memory, GLuint64 offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedBufferStorageMemEXT', buffer, size, memory, uint64(offset) );

return
