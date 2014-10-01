function glNamedCopyBufferSubDataEXT( readBuffer, writeBuffer, readOffset, writeOffset, size )

% glNamedCopyBufferSubDataEXT  Interface to OpenGL function glNamedCopyBufferSubDataEXT
%
% usage:  glNamedCopyBufferSubDataEXT( readBuffer, writeBuffer, readOffset, writeOffset, size )
%
% C function:  void glNamedCopyBufferSubDataEXT(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glNamedCopyBufferSubDataEXT', readBuffer, writeBuffer, readOffset, writeOffset, size );

return
