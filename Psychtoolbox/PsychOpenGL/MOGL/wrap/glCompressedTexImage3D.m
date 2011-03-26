function glCompressedTexImage3D( target, level, internalformat, width, height, depth, border, imageSize, data )

% glCompressedTexImage3D  Interface to OpenGL function glCompressedTexImage3D
%
% usage:  glCompressedTexImage3D( target, level, internalformat, width, height, depth, border, imageSize, data )
%
% C function:  void glCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid* data)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTexImage3D', target, level, internalformat, width, height, depth, border, imageSize, data );

return
