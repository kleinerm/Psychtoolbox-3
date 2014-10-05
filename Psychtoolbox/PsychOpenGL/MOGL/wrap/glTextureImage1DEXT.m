function glTextureImage1DEXT( texture, target, level, internalformat, width, border, format, type, pixels )

% glTextureImage1DEXT  Interface to OpenGL function glTextureImage1DEXT
%
% usage:  glTextureImage1DEXT( texture, target, level, internalformat, width, border, format, type, pixels )
%
% C function:  void glTextureImage1DEXT(GLuint texture, GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glTextureImage1DEXT', texture, target, level, internalformat, width, border, format, type, pixels );

return
