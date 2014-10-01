function glTextureImage2DMultisampleNV( texture, target, samples, internalFormat, width, height, fixedSampleLocations )

% glTextureImage2DMultisampleNV  Interface to OpenGL function glTextureImage2DMultisampleNV
%
% usage:  glTextureImage2DMultisampleNV( texture, target, samples, internalFormat, width, height, fixedSampleLocations )
%
% C function:  void glTextureImage2DMultisampleNV(GLuint texture, GLenum target, GLsizei samples, GLint internalFormat, GLsizei width, GLsizei height, GLboolean fixedSampleLocations)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glTextureImage2DMultisampleNV', texture, target, samples, internalFormat, width, height, fixedSampleLocations );

return
