function glCompressedMultiTexSubImage1DEXT( texunit, target, level, xoffset, width, format, imageSize, bits )

% glCompressedMultiTexSubImage1DEXT  Interface to OpenGL function glCompressedMultiTexSubImage1DEXT
%
% usage:  glCompressedMultiTexSubImage1DEXT( texunit, target, level, xoffset, width, format, imageSize, bits )
%
% C function:  void glCompressedMultiTexSubImage1DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* bits)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glCompressedMultiTexSubImage1DEXT', texunit, target, level, xoffset, width, format, imageSize, bits );

return
