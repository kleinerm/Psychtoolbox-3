function glTexStorageMem3DMultisampleEXT( target, samples, internalFormat, width, height, depth, fixedSampleLocations, memory, offset )

% glTexStorageMem3DMultisampleEXT  Interface to OpenGL function glTexStorageMem3DMultisampleEXT
%
% usage:  glTexStorageMem3DMultisampleEXT( target, samples, internalFormat, width, height, depth, fixedSampleLocations, memory, offset )
%
% C function:  void glTexStorageMem3DMultisampleEXT(GLenum target, GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedSampleLocations, GLuint memory, GLuint64 offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glTexStorageMem3DMultisampleEXT', target, samples, internalFormat, width, height, depth, fixedSampleLocations, memory, uint64(offset) );

return
