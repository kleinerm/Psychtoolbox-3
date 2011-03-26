function glCompressedTexSubImage1D( target, level, xoffset, width, format, imageSize, data )

% glCompressedTexSubImage1D  Interface to OpenGL function glCompressedTexSubImage1D
%
% usage:  glCompressedTexSubImage1D( target, level, xoffset, width, format, imageSize, data )
%
% C function:  void glCompressedTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid* data)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glCompressedTexSubImage1D', target, level, xoffset, width, format, imageSize, data );

return
