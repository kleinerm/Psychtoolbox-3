function glCompressedTexImage1DARB( target, level, internalformat, width, border, imageSize, data )

% glCompressedTexImage1DARB  Interface to OpenGL function glCompressedTexImage1DARB
%
% usage:  glCompressedTexImage1DARB( target, level, internalformat, width, border, imageSize, data )
%
% C function:  void glCompressedTexImage1DARB(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTexImage1DARB', target, level, internalformat, width, border, imageSize, data );

return
