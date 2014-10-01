function glCompressedTextureSubImage1DEXT( texture, target, level, xoffset, width, format, imageSize, bits )

% glCompressedTextureSubImage1DEXT  Interface to OpenGL function glCompressedTextureSubImage1DEXT
%
% usage:  glCompressedTextureSubImage1DEXT( texture, target, level, xoffset, width, format, imageSize, bits )
%
% C function:  void glCompressedTextureSubImage1DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* bits)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTextureSubImage1DEXT', texture, target, level, xoffset, width, format, imageSize, bits );

return
