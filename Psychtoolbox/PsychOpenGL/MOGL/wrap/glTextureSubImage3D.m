function glTextureSubImage3D( texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels )

% glTextureSubImage3D  Interface to OpenGL function glTextureSubImage3D
%
% usage:  glTextureSubImage3D( texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels )
%
% C function:  void glTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=11,
    error('invalid number of arguments');
end

moglcore( 'glTextureSubImage3D', texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels );

return
