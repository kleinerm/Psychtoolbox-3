function glCompressedMultiTexImage1DEXT( texunit, target, level, internalformat, width, border, imageSize, bits )

% glCompressedMultiTexImage1DEXT  Interface to OpenGL function glCompressedMultiTexImage1DEXT
%
% usage:  glCompressedMultiTexImage1DEXT( texunit, target, level, internalformat, width, border, imageSize, bits )
%
% C function:  void glCompressedMultiTexImage1DEXT(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void* bits)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glCompressedMultiTexImage1DEXT', texunit, target, level, internalformat, width, border, imageSize, bits );

return
