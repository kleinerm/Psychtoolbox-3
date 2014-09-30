function glCompressedMultiTexImage3DEXT( texunit, target, level, internalformat, width, height, depth, border, imageSize, bits )

% glCompressedMultiTexImage3DEXT  Interface to OpenGL function glCompressedMultiTexImage3DEXT
%
% usage:  glCompressedMultiTexImage3DEXT( texunit, target, level, internalformat, width, height, depth, border, imageSize, bits )
%
% C function:  void glCompressedMultiTexImage3DEXT(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* bits)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=10,
    error('invalid number of arguments');
end

moglcore( 'glCompressedMultiTexImage3DEXT', texunit, target, level, internalformat, width, height, depth, border, imageSize, bits );

return
