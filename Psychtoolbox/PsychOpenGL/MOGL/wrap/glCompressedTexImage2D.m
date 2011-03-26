function glCompressedTexImage2D( target, level, internalformat, width, height, border, imageSize, data )

% glCompressedTexImage2D  Interface to OpenGL function glCompressedTexImage2D
%
% usage:  glCompressedTexImage2D( target, level, internalformat, width, height, border, imageSize, data )
%
% C function:  void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTexImage2D', target, level, internalformat, width, height, border, imageSize, data );

return
