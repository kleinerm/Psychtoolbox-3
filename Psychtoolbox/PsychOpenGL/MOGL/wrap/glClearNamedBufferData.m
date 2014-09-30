function glClearNamedBufferData( buffer, internalformat, format, type, data )

% glClearNamedBufferData  Interface to OpenGL function glClearNamedBufferData
%
% usage:  glClearNamedBufferData( buffer, internalformat, format, type, data )
%
% C function:  void glClearNamedBufferData(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glClearNamedBufferData', buffer, internalformat, format, type, data );

return
