function glCompressedTexSubImage2D( target, level, xoffset, yoffset, width, height, format, imageSize, data )

% glCompressedTexSubImage2D  Interface to OpenGL function glCompressedTexSubImage2D
%
% usage:  glCompressedTexSubImage2D( target, level, xoffset, yoffset, width, height, format, imageSize, data )
%
% C function:  void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTexSubImage2D', target, level, xoffset, yoffset, width, height, format, imageSize, data );

return
