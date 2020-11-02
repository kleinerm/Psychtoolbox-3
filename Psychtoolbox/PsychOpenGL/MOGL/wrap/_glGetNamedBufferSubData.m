function data = glGetNamedBufferSubData( buffer, offset, size )

% glGetNamedBufferSubData  Interface to OpenGL function glGetNamedBufferSubData
%
% usage:  data = glGetNamedBufferSubData( buffer, offset, size )
%
% C function:  void glGetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

data = (0);

moglcore( 'glGetNamedBufferSubData', buffer, offset, size, data );

return
