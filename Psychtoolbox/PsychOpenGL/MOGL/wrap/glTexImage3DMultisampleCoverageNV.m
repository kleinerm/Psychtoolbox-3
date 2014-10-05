function glTexImage3DMultisampleCoverageNV( target, coverageSamples, colorSamples, internalFormat, width, height, depth, fixedSampleLocations )

% glTexImage3DMultisampleCoverageNV  Interface to OpenGL function glTexImage3DMultisampleCoverageNV
%
% usage:  glTexImage3DMultisampleCoverageNV( target, coverageSamples, colorSamples, internalFormat, width, height, depth, fixedSampleLocations )
%
% C function:  void glTexImage3DMultisampleCoverageNV(GLenum target, GLsizei coverageSamples, GLsizei colorSamples, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedSampleLocations)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glTexImage3DMultisampleCoverageNV', target, coverageSamples, colorSamples, internalFormat, width, height, depth, fixedSampleLocations );

return
