function glCompressedTexImage2DARB( target, level, internalformat, width, height, border, imageSize, data )

% glCompressedTexImage2DARB  Interface to OpenGL function glCompressedTexImage2DARB
%
% usage:  glCompressedTexImage2DARB( target, level, internalformat, width, height, border, imageSize, data )
%
% C function:  void glCompressedTexImage2DARB(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTexImage2DARB', target, level, internalformat, width, height, border, imageSize, data );

return
