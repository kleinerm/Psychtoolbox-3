function glCompressedTextureSubImage3DEXT( texture, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, bits )

% glCompressedTextureSubImage3DEXT  Interface to OpenGL function glCompressedTextureSubImage3DEXT
%
% usage:  glCompressedTextureSubImage3DEXT( texture, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, bits )
%
% C function:  void glCompressedTextureSubImage3DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* bits)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=12,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTextureSubImage3DEXT', texture, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, bits );

return
