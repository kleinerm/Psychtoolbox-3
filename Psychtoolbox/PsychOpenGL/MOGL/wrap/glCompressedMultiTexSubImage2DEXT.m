function glCompressedMultiTexSubImage2DEXT( texunit, target, level, xoffset, yoffset, width, height, format, imageSize, bits )

% glCompressedMultiTexSubImage2DEXT  Interface to OpenGL function glCompressedMultiTexSubImage2DEXT
%
% usage:  glCompressedMultiTexSubImage2DEXT( texunit, target, level, xoffset, yoffset, width, height, format, imageSize, bits )
%
% C function:  void glCompressedMultiTexSubImage2DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* bits)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=10,
    error('invalid number of arguments');
end

moglcore( 'glCompressedMultiTexSubImage2DEXT', texunit, target, level, xoffset, yoffset, width, height, format, imageSize, bits );

return
