function glCompressedTexImage1D( target, level, internalformat, width, border, imageSize, data )

% glCompressedTexImage1D  Interface to OpenGL function glCompressedTexImage1D
%
% usage:  glCompressedTexImage1D( target, level, internalformat, width, border, imageSize, data )
%
% C function:  void glCompressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid* data)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTexImage1D', target, level, internalformat, width, border, imageSize, data );

return
