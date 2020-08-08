function glLGPUNamedBufferSubDataNVX( gpuMask, buffer, offset, size, data )

% glLGPUNamedBufferSubDataNVX  Interface to OpenGL function glLGPUNamedBufferSubDataNVX
%
% usage:  glLGPUNamedBufferSubDataNVX( gpuMask, buffer, offset, size, data )
%
% C function:  void glLGPUNamedBufferSubDataNVX(GLbitfield gpuMask, GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glLGPUNamedBufferSubDataNVX', gpuMask, buffer, offset, size, data );

return
