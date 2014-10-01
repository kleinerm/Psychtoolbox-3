function glCompressedMultiTexImage2DEXT( texunit, target, level, internalformat, width, height, border, imageSize, bits )

% glCompressedMultiTexImage2DEXT  Interface to OpenGL function glCompressedMultiTexImage2DEXT
%
% usage:  glCompressedMultiTexImage2DEXT( texunit, target, level, internalformat, width, height, border, imageSize, bits )
%
% C function:  void glCompressedMultiTexImage2DEXT(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* bits)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glCompressedMultiTexImage2DEXT', texunit, target, level, internalformat, width, height, border, imageSize, bits );

return
