function glClearNamedBufferSubData( buffer, internalformat, offset, size, format, type, data )

% glClearNamedBufferSubData  Interface to OpenGL function glClearNamedBufferSubData
%
% usage:  glClearNamedBufferSubData( buffer, internalformat, offset, size, format, type, data )
%
% C function:  void glClearNamedBufferSubData(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizei size, GLenum format, GLenum type, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glClearNamedBufferSubData', buffer, internalformat, offset, size, format, type, data );

return
