function glTexStorageMem2DMultisampleEXT( target, samples, internalFormat, width, height, fixedSampleLocations, memory, offset )

% glTexStorageMem2DMultisampleEXT  Interface to OpenGL function glTexStorageMem2DMultisampleEXT
%
% usage:  glTexStorageMem2DMultisampleEXT( target, samples, internalFormat, width, height, fixedSampleLocations, memory, offset )
%
% C function:  void glTexStorageMem2DMultisampleEXT(GLenum target, GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height, GLboolean fixedSampleLocations, GLuint memory, GLuint64 offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glTexStorageMem2DMultisampleEXT', target, samples, internalFormat, width, height, fixedSampleLocations, memory, uint64(offset) );

return
