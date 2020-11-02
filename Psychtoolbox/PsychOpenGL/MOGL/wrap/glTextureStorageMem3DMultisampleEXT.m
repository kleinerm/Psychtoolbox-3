function glTextureStorageMem3DMultisampleEXT( texture, samples, internalFormat, width, height, depth, fixedSampleLocations, memory, offset )

% glTextureStorageMem3DMultisampleEXT  Interface to OpenGL function glTextureStorageMem3DMultisampleEXT
%
% usage:  glTextureStorageMem3DMultisampleEXT( texture, samples, internalFormat, width, height, depth, fixedSampleLocations, memory, offset )
%
% C function:  void glTextureStorageMem3DMultisampleEXT(GLuint texture, GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedSampleLocations, GLuint memory, GLuint64 offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glTextureStorageMem3DMultisampleEXT', texture, samples, internalFormat, width, height, depth, fixedSampleLocations, memory, uint64(offset) );

return
