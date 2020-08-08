function glSignalSemaphoreEXT( semaphore, numBufferBarriers, buffers, numTextureBarriers, textures, dstLayouts )

% glSignalSemaphoreEXT  Interface to OpenGL function glSignalSemaphoreEXT
%
% usage:  glSignalSemaphoreEXT( semaphore, numBufferBarriers, buffers, numTextureBarriers, textures, dstLayouts )
%
% C function:  void glSignalSemaphoreEXT(GLuint semaphore, GLuint numBufferBarriers, const GLuint* buffers, GLuint numTextureBarriers, const GLuint* textures, const GLenum* dstLayouts)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glSignalSemaphoreEXT', semaphore, numBufferBarriers, uint32(buffers), numTextureBarriers, uint32(textures), uint32(dstLayouts) );

return
