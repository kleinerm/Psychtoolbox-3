function glCompressedTextureSubImage1D( texture, level, xoffset, width, format, imageSize, data )

% glCompressedTextureSubImage1D  Interface to OpenGL function glCompressedTextureSubImage1D
%
% usage:  glCompressedTextureSubImage1D( texture, level, xoffset, width, format, imageSize, data )
%
% C function:  void glCompressedTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTextureSubImage1D', texture, level, xoffset, width, format, imageSize, data );

return
