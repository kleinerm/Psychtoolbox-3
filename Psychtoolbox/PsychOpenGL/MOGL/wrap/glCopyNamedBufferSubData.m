function glCopyNamedBufferSubData( readBuffer, writeBuffer, readOffset, writeOffset, size )

% glCopyNamedBufferSubData  Interface to OpenGL function glCopyNamedBufferSubData
%
% usage:  glCopyNamedBufferSubData( readBuffer, writeBuffer, readOffset, writeOffset, size )
%
% C function:  void glCopyNamedBufferSubData(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizei size)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glCopyNamedBufferSubData', readBuffer, writeBuffer, readOffset, writeOffset, size );

return
