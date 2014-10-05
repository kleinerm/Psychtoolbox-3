function glNamedBufferStorageEXT( buffer, size, data, flags )

% glNamedBufferStorageEXT  Interface to OpenGL function glNamedBufferStorageEXT
%
% usage:  glNamedBufferStorageEXT( buffer, size, data, flags )
%
% C function:  void glNamedBufferStorageEXT(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedBufferStorageEXT', buffer, size, data, flags );

return
