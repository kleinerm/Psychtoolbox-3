function glCompressedTexImage3DARB( target, level, internalformat, width, height, depth, border, imageSize, data )

% glCompressedTexImage3DARB  Interface to OpenGL function glCompressedTexImage3DARB
%
% usage:  glCompressedTexImage3DARB( target, level, internalformat, width, height, depth, border, imageSize, data )
%
% C function:  void glCompressedTexImage3DARB(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTexImage3DARB', target, level, internalformat, width, height, depth, border, imageSize, data );

return
