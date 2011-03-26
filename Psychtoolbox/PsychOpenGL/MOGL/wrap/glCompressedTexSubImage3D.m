function glCompressedTexSubImage3D( target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data )

% glCompressedTexSubImage3D  Interface to OpenGL function glCompressedTexSubImage3D
%
% usage:  glCompressedTexSubImage3D( target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data )
%
% C function:  void glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid* data)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=11,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTexSubImage3D', target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data );

return
