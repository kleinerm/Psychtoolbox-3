function pixels = glGetCompressedTextureSubImage( texture, level, xoffset, yoffset, zoffset, width, height, depth, bufSize )

% glGetCompressedTextureSubImage  Interface to OpenGL function glGetCompressedTextureSubImage
%
% usage:  pixels = glGetCompressedTextureSubImage( texture, level, xoffset, yoffset, zoffset, width, height, depth, bufSize )
%
% C function:  void glGetCompressedTextureSubImage(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei bufSize, void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=9,
    error('invalid number of arguments');
end

pixels = (0);

moglcore( 'glGetCompressedTextureSubImage', texture, level, xoffset, yoffset, zoffset, width, height, depth, bufSize, pixels );

return
