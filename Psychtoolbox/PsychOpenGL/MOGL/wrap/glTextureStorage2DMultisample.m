function glTextureStorage2DMultisample( texture, samples, internalformat, width, height, fixedsamplelocations )

% glTextureStorage2DMultisample  Interface to OpenGL function glTextureStorage2DMultisample
%
% usage:  glTextureStorage2DMultisample( texture, samples, internalformat, width, height, fixedsamplelocations )
%
% C function:  void glTextureStorage2DMultisample(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glTextureStorage2DMultisample', texture, samples, internalformat, width, height, fixedsamplelocations );

return
