function glWaitSemaphoreEXT( semaphore, numBufferBarriers, buffers, numTextureBarriers, textures, srcLayouts )

% glWaitSemaphoreEXT  Interface to OpenGL function glWaitSemaphoreEXT
%
% usage:  glWaitSemaphoreEXT( semaphore, numBufferBarriers, buffers, numTextureBarriers, textures, srcLayouts )
%
% C function:  void glWaitSemaphoreEXT(GLuint semaphore, GLuint numBufferBarriers, const GLuint* buffers, GLuint numTextureBarriers, const GLuint* textures, const GLenum* srcLayouts)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glWaitSemaphoreEXT', semaphore, numBufferBarriers, uint32(buffers), numTextureBarriers, uint32(textures), uint32(srcLayouts) );

return
