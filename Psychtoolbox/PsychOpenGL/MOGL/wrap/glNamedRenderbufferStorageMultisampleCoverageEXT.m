function glNamedRenderbufferStorageMultisampleCoverageEXT( renderbuffer, coverageSamples, colorSamples, internalformat, width, height )

% glNamedRenderbufferStorageMultisampleCoverageEXT  Interface to OpenGL function glNamedRenderbufferStorageMultisampleCoverageEXT
%
% usage:  glNamedRenderbufferStorageMultisampleCoverageEXT( renderbuffer, coverageSamples, colorSamples, internalformat, width, height )
%
% C function:  void glNamedRenderbufferStorageMultisampleCoverageEXT(GLuint renderbuffer, GLsizei coverageSamples, GLsizei colorSamples, GLenum internalformat, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glNamedRenderbufferStorageMultisampleCoverageEXT', renderbuffer, coverageSamples, colorSamples, internalformat, width, height );

return
