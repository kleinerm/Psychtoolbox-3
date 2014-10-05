function glCompressedTextureImage1DEXT( texture, target, level, internalformat, width, border, imageSize, bits )

% glCompressedTextureImage1DEXT  Interface to OpenGL function glCompressedTextureImage1DEXT
%
% usage:  glCompressedTextureImage1DEXT( texture, target, level, internalformat, width, border, imageSize, bits )
%
% C function:  void glCompressedTextureImage1DEXT(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void* bits)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTextureImage1DEXT', texture, target, level, internalformat, width, border, imageSize, bits );

return
