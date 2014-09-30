function glCompressedTexSubImage3DARB( target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data )

% glCompressedTexSubImage3DARB  Interface to OpenGL function glCompressedTexSubImage3DARB
%
% usage:  glCompressedTexSubImage3DARB( target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data )
%
% C function:  void glCompressedTexSubImage3DARB(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=11,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTexSubImage3DARB', target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data );

return
