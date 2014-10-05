function glTextureSubImage2D( texture, level, xoffset, yoffset, width, height, format, type, pixels )

% glTextureSubImage2D  Interface to OpenGL function glTextureSubImage2D
%
% usage:  glTextureSubImage2D( texture, level, xoffset, yoffset, width, height, format, type, pixels )
%
% C function:  void glTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glTextureSubImage2D', texture, level, xoffset, yoffset, width, height, format, type, pixels );

return
