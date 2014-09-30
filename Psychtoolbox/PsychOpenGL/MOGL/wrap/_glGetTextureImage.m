function pixels = glGetTextureImage( texture, level, format, type, bufSize )

% glGetTextureImage  Interface to OpenGL function glGetTextureImage
%
% usage:  pixels = glGetTextureImage( texture, level, format, type, bufSize )
%
% C function:  void glGetTextureImage(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=5,
    error('invalid number of arguments');
end

pixels = (0);

moglcore( 'glGetTextureImage', texture, level, format, type, bufSize, pixels );

return
