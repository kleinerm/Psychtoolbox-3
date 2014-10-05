function glTextureSubImage1D( texture, level, xoffset, width, format, type, pixels )

% glTextureSubImage1D  Interface to OpenGL function glTextureSubImage1D
%
% usage:  glTextureSubImage1D( texture, level, xoffset, width, format, type, pixels )
%
% C function:  void glTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glTextureSubImage1D', texture, level, xoffset, width, format, type, pixels );

return
