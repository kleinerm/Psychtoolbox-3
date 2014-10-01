function glNamedBufferSubDataEXT( buffer, offset, size, data )

% glNamedBufferSubDataEXT  Interface to OpenGL function glNamedBufferSubDataEXT
%
% usage:  glNamedBufferSubDataEXT( buffer, offset, size, data )
%
% C function:  void glNamedBufferSubDataEXT(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedBufferSubDataEXT', buffer, offset, size, data );

return
