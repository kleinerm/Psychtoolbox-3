function glTextureStorageMem2DMultisampleEXT( texture, samples, internalFormat, width, height, fixedSampleLocations, memory, offset )

% glTextureStorageMem2DMultisampleEXT  Interface to OpenGL function glTextureStorageMem2DMultisampleEXT
%
% usage:  glTextureStorageMem2DMultisampleEXT( texture, samples, internalFormat, width, height, fixedSampleLocations, memory, offset )
%
% C function:  void glTextureStorageMem2DMultisampleEXT(GLuint texture, GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height, GLboolean fixedSampleLocations, GLuint memory, GLuint64 offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glTextureStorageMem2DMultisampleEXT', texture, samples, internalFormat, width, height, fixedSampleLocations, memory, uint64(offset) );

return
