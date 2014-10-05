function glCompressedTextureSubImage2DEXT( texture, target, level, xoffset, yoffset, width, height, format, imageSize, bits )

% glCompressedTextureSubImage2DEXT  Interface to OpenGL function glCompressedTextureSubImage2DEXT
%
% usage:  glCompressedTextureSubImage2DEXT( texture, target, level, xoffset, yoffset, width, height, format, imageSize, bits )
%
% C function:  void glCompressedTextureSubImage2DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* bits)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=10,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTextureSubImage2DEXT', texture, target, level, xoffset, yoffset, width, height, format, imageSize, bits );

return
