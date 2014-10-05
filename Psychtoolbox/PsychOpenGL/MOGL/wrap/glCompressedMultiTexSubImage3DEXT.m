function glCompressedMultiTexSubImage3DEXT( texunit, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, bits )

% glCompressedMultiTexSubImage3DEXT  Interface to OpenGL function glCompressedMultiTexSubImage3DEXT
%
% usage:  glCompressedMultiTexSubImage3DEXT( texunit, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, bits )
%
% C function:  void glCompressedMultiTexSubImage3DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* bits)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=12,
    error('invalid number of arguments');
end

moglcore( 'glCompressedMultiTexSubImage3DEXT', texunit, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, bits );

return
