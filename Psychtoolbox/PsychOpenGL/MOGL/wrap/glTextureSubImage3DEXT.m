function glTextureSubImage3DEXT( texture, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels )

% glTextureSubImage3DEXT  Interface to OpenGL function glTextureSubImage3DEXT
%
% usage:  glTextureSubImage3DEXT( texture, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels )
%
% C function:  void glTextureSubImage3DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=12,
    error('invalid number of arguments');
end

moglcore( 'glTextureSubImage3DEXT', texture, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels );

return
