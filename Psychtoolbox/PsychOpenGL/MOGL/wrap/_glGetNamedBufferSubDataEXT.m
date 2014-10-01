function data = glGetNamedBufferSubDataEXT( buffer, offset, size )

% glGetNamedBufferSubDataEXT  Interface to OpenGL function glGetNamedBufferSubDataEXT
%
% usage:  data = glGetNamedBufferSubDataEXT( buffer, offset, size )
%
% C function:  void glGetNamedBufferSubDataEXT(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

data = (0);

moglcore( 'glGetNamedBufferSubDataEXT', buffer, offset, size, data );

return
