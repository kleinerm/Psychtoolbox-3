function glTextureSubImage1DEXT( texture, target, level, xoffset, width, format, type, pixels )

% glTextureSubImage1DEXT  Interface to OpenGL function glTextureSubImage1DEXT
%
% usage:  glTextureSubImage1DEXT( texture, target, level, xoffset, width, format, type, pixels )
%
% C function:  void glTextureSubImage1DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glTextureSubImage1DEXT', texture, target, level, xoffset, width, format, type, pixels );

return
