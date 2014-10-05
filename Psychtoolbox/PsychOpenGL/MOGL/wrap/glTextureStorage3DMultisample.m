function glTextureStorage3DMultisample( texture, samples, internalformat, width, height, depth, fixedsamplelocations )

% glTextureStorage3DMultisample  Interface to OpenGL function glTextureStorage3DMultisample
%
% usage:  glTextureStorage3DMultisample( texture, samples, internalformat, width, height, depth, fixedsamplelocations )
%
% C function:  void glTextureStorage3DMultisample(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glTextureStorage3DMultisample', texture, samples, internalformat, width, height, depth, fixedsamplelocations );

return
