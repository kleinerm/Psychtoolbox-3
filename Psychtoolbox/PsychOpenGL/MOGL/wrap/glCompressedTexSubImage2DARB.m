function glCompressedTexSubImage2DARB( target, level, xoffset, yoffset, width, height, format, imageSize, data )

% glCompressedTexSubImage2DARB  Interface to OpenGL function glCompressedTexSubImage2DARB
%
% usage:  glCompressedTexSubImage2DARB( target, level, xoffset, yoffset, width, height, format, imageSize, data )
%
% C function:  void glCompressedTexSubImage2DARB(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTexSubImage2DARB', target, level, xoffset, yoffset, width, height, format, imageSize, data );

return
