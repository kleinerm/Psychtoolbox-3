function glTextureImage3DMultisampleNV( texture, target, samples, internalFormat, width, height, depth, fixedSampleLocations )

% glTextureImage3DMultisampleNV  Interface to OpenGL function glTextureImage3DMultisampleNV
%
% usage:  glTextureImage3DMultisampleNV( texture, target, samples, internalFormat, width, height, depth, fixedSampleLocations )
%
% C function:  void glTextureImage3DMultisampleNV(GLuint texture, GLenum target, GLsizei samples, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedSampleLocations)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glTextureImage3DMultisampleNV', texture, target, samples, internalFormat, width, height, depth, fixedSampleLocations );

return
