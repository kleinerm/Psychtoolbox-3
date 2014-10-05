function glCompressedTextureSubImage3D( texture, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data )

% glCompressedTextureSubImage3D  Interface to OpenGL function glCompressedTextureSubImage3D
%
% usage:  glCompressedTextureSubImage3D( texture, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data )
%
% C function:  void glCompressedTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=11,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTextureSubImage3D', texture, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data );

return
