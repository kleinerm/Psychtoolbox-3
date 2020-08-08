function glMulticastBufferSubDataNV( gpuMask, buffer, offset, size, data )

% glMulticastBufferSubDataNV  Interface to OpenGL function glMulticastBufferSubDataNV
%
% usage:  glMulticastBufferSubDataNV( gpuMask, buffer, offset, size, data )
%
% C function:  void glMulticastBufferSubDataNV(GLbitfield gpuMask, GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMulticastBufferSubDataNV', gpuMask, buffer, offset, size, data );

return
