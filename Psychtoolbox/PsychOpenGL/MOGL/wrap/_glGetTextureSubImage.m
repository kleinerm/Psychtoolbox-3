function pixels = glGetTextureSubImage( texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, bufSize )

% glGetTextureSubImage  Interface to OpenGL function glGetTextureSubImage
%
% usage:  pixels = glGetTextureSubImage( texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, bufSize )
%
% C function:  void glGetTextureSubImage(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=11,
    error('invalid number of arguments');
end

pixels = (0);

moglcore( 'glGetTextureSubImage', texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, bufSize, pixels );

return
