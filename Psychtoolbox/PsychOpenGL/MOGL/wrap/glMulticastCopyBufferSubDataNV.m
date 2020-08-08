function glMulticastCopyBufferSubDataNV( readGpu, writeGpuMask, readBuffer, writeBuffer, readOffset, writeOffset, size )

% glMulticastCopyBufferSubDataNV  Interface to OpenGL function glMulticastCopyBufferSubDataNV
%
% usage:  glMulticastCopyBufferSubDataNV( readGpu, writeGpuMask, readBuffer, writeBuffer, readOffset, writeOffset, size )
%
% C function:  void glMulticastCopyBufferSubDataNV(GLuint readGpu, GLbitfield writeGpuMask, GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glMulticastCopyBufferSubDataNV', readGpu, writeGpuMask, readBuffer, writeBuffer, readOffset, writeOffset, size );

return
