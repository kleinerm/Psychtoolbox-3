function glTextureImage2DEXT( texture, target, level, internalformat, width, height, border, format, type, pixels )

% glTextureImage2DEXT  Interface to OpenGL function glTextureImage2DEXT
%
% usage:  glTextureImage2DEXT( texture, target, level, internalformat, width, height, border, format, type, pixels )
%
% C function:  void glTextureImage2DEXT(GLuint texture, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=10,
    error('invalid number of arguments');
end

moglcore( 'glTextureImage2DEXT', texture, target, level, internalformat, width, height, border, format, type, pixels );

return
