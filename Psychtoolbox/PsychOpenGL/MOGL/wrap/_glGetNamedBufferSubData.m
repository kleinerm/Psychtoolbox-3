function data = glGetNamedBufferSubData( buffer, offset, size )

% glGetNamedBufferSubData  Interface to OpenGL function glGetNamedBufferSubData
%
% usage:  data = glGetNamedBufferSubData( buffer, offset, size )
%
% C function:  void glGetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizei size, void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

data = (0);

moglcore( 'glGetNamedBufferSubData', buffer, offset, size, data );

return
