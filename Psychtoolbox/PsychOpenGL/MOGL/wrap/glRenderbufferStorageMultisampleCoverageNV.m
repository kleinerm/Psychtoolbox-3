function glRenderbufferStorageMultisampleCoverageNV( target, coverageSamples, colorSamples, internalformat, width, height )

% glRenderbufferStorageMultisampleCoverageNV  Interface to OpenGL function glRenderbufferStorageMultisampleCoverageNV
%
% usage:  glRenderbufferStorageMultisampleCoverageNV( target, coverageSamples, colorSamples, internalformat, width, height )
%
% C function:  void glRenderbufferStorageMultisampleCoverageNV(GLenum target, GLsizei coverageSamples, GLsizei colorSamples, GLenum internalformat, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glRenderbufferStorageMultisampleCoverageNV', target, coverageSamples, colorSamples, internalformat, width, height );

return
