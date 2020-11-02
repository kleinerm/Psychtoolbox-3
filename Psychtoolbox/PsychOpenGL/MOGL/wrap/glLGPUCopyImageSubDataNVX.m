function glLGPUCopyImageSubDataNVX( sourceGpu, destinationGpuMask, srcName, srcTarget, srcLevel, srcX, srxY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, width, height, depth )

% glLGPUCopyImageSubDataNVX  Interface to OpenGL function glLGPUCopyImageSubDataNVX
%
% usage:  glLGPUCopyImageSubDataNVX( sourceGpu, destinationGpuMask, srcName, srcTarget, srcLevel, srcX, srxY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, width, height, depth )
%
% C function:  void glLGPUCopyImageSubDataNVX(GLuint sourceGpu, GLbitfield destinationGpuMask, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srxY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei width, GLsizei height, GLsizei depth)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=17,
    error('invalid number of arguments');
end

moglcore( 'glLGPUCopyImageSubDataNVX', sourceGpu, destinationGpuMask, srcName, srcTarget, srcLevel, srcX, srxY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, width, height, depth );

return
