function glCompressedTextureImage3DEXT( texture, target, level, internalformat, width, height, depth, border, imageSize, bits )

% glCompressedTextureImage3DEXT  Interface to OpenGL function glCompressedTextureImage3DEXT
%
% usage:  glCompressedTextureImage3DEXT( texture, target, level, internalformat, width, height, depth, border, imageSize, bits )
%
% C function:  void glCompressedTextureImage3DEXT(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* bits)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=10,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTextureImage3DEXT', texture, target, level, internalformat, width, height, depth, border, imageSize, bits );

return
