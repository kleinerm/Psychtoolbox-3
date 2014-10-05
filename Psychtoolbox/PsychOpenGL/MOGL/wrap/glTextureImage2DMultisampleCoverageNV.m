function glTextureImage2DMultisampleCoverageNV( texture, target, coverageSamples, colorSamples, internalFormat, width, height, fixedSampleLocations )

% glTextureImage2DMultisampleCoverageNV  Interface to OpenGL function glTextureImage2DMultisampleCoverageNV
%
% usage:  glTextureImage2DMultisampleCoverageNV( texture, target, coverageSamples, colorSamples, internalFormat, width, height, fixedSampleLocations )
%
% C function:  void glTextureImage2DMultisampleCoverageNV(GLuint texture, GLenum target, GLsizei coverageSamples, GLsizei colorSamples, GLint internalFormat, GLsizei width, GLsizei height, GLboolean fixedSampleLocations)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glTextureImage2DMultisampleCoverageNV', texture, target, coverageSamples, colorSamples, internalFormat, width, height, fixedSampleLocations );

return
