function glTexImage2DMultisampleCoverageNV( target, coverageSamples, colorSamples, internalFormat, width, height, fixedSampleLocations )

% glTexImage2DMultisampleCoverageNV  Interface to OpenGL function glTexImage2DMultisampleCoverageNV
%
% usage:  glTexImage2DMultisampleCoverageNV( target, coverageSamples, colorSamples, internalFormat, width, height, fixedSampleLocations )
%
% C function:  void glTexImage2DMultisampleCoverageNV(GLenum target, GLsizei coverageSamples, GLsizei colorSamples, GLint internalFormat, GLsizei width, GLsizei height, GLboolean fixedSampleLocations)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glTexImage2DMultisampleCoverageNV', target, coverageSamples, colorSamples, internalFormat, width, height, fixedSampleLocations );

return
