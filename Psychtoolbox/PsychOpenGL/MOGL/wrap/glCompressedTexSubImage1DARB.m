function glCompressedTexSubImage1DARB( target, level, xoffset, width, format, imageSize, data )

% glCompressedTexSubImage1DARB  Interface to OpenGL function glCompressedTexSubImage1DARB
%
% usage:  glCompressedTexSubImage1DARB( target, level, xoffset, width, format, imageSize, data )
%
% C function:  void glCompressedTexSubImage1DARB(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTexSubImage1DARB', target, level, xoffset, width, format, imageSize, data );

return
