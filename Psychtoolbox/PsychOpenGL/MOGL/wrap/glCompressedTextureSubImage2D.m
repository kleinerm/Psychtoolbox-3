function glCompressedTextureSubImage2D( texture, level, xoffset, yoffset, width, height, format, imageSize, data )

% glCompressedTextureSubImage2D  Interface to OpenGL function glCompressedTextureSubImage2D
%
% usage:  glCompressedTextureSubImage2D( texture, level, xoffset, yoffset, width, height, format, imageSize, data )
%
% C function:  void glCompressedTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTextureSubImage2D', texture, level, xoffset, yoffset, width, height, format, imageSize, data );

return
