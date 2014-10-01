function glTextureImage3DEXT( texture, target, level, internalformat, width, height, depth, border, format, type, pixels )

% glTextureImage3DEXT  Interface to OpenGL function glTextureImage3DEXT
%
% usage:  glTextureImage3DEXT( texture, target, level, internalformat, width, height, depth, border, format, type, pixels )
%
% C function:  void glTextureImage3DEXT(GLuint texture, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=11,
    error('invalid number of arguments');
end

moglcore( 'glTextureImage3DEXT', texture, target, level, internalformat, width, height, depth, border, format, type, pixels );

return
