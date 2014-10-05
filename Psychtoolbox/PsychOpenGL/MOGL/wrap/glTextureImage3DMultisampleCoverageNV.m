function glTextureImage3DMultisampleCoverageNV( texture, target, coverageSamples, colorSamples, internalFormat, width, height, depth, fixedSampleLocations )

% glTextureImage3DMultisampleCoverageNV  Interface to OpenGL function glTextureImage3DMultisampleCoverageNV
%
% usage:  glTextureImage3DMultisampleCoverageNV( texture, target, coverageSamples, colorSamples, internalFormat, width, height, depth, fixedSampleLocations )
%
% C function:  void glTextureImage3DMultisampleCoverageNV(GLuint texture, GLenum target, GLsizei coverageSamples, GLsizei colorSamples, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedSampleLocations)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glTextureImage3DMultisampleCoverageNV', texture, target, coverageSamples, colorSamples, internalFormat, width, height, depth, fixedSampleLocations );

return
