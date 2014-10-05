function glCompressedTextureImage2DEXT( texture, target, level, internalformat, width, height, border, imageSize, bits )

% glCompressedTextureImage2DEXT  Interface to OpenGL function glCompressedTextureImage2DEXT
%
% usage:  glCompressedTextureImage2DEXT( texture, target, level, internalformat, width, height, border, imageSize, bits )
%
% C function:  void glCompressedTextureImage2DEXT(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* bits)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTextureImage2DEXT', texture, target, level, internalformat, width, height, border, imageSize, bits );

return
