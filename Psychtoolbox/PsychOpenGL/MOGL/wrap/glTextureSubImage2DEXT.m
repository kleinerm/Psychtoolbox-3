function glTextureSubImage2DEXT( texture, target, level, xoffset, yoffset, width, height, format, type, pixels )

% glTextureSubImage2DEXT  Interface to OpenGL function glTextureSubImage2DEXT
%
% usage:  glTextureSubImage2DEXT( texture, target, level, xoffset, yoffset, width, height, format, type, pixels )
%
% C function:  void glTextureSubImage2DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=10,
    error('invalid number of arguments');
end

moglcore( 'glTextureSubImage2DEXT', texture, target, level, xoffset, yoffset, width, height, format, type, pixels );

return
