function glClearNamedBufferSubDataEXT( buffer, internalformat, format, type, ptr1, ptr2, data )

% glClearNamedBufferSubDataEXT  Interface to OpenGL function glClearNamedBufferSubDataEXT
%
% usage:  glClearNamedBufferSubDataEXT( buffer, internalformat, format, type, ptr1, ptr2, data )
%
% C function:  void glClearNamedBufferSubDataEXT(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, GLsizei ptr, GLsizei ptr, const void* data)

% 30-Aug-2012 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glClearNamedBufferSubDataEXT', buffer, internalformat, format, type, ptr1, ptr2, data );

return
