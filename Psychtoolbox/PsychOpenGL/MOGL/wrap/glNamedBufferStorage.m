function glNamedBufferStorage( buffer, size, data, flags )

% glNamedBufferStorage  Interface to OpenGL function glNamedBufferStorage
%
% usage:  glNamedBufferStorage( buffer, size, data, flags )
%
% C function:  void glNamedBufferStorage(GLuint buffer, GLsizei size, const void* data, GLbitfield flags)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedBufferStorage', buffer, size, data, flags );

return
