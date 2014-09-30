function glTexSubImage3DEXT( target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels )

% glTexSubImage3DEXT  Interface to OpenGL function glTexSubImage3DEXT
%
% usage:  glTexSubImage3DEXT( target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels )
%
% C function:  void glTexSubImage3DEXT(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=11,
    error('invalid number of arguments');
end

moglcore( 'glTexSubImage3DEXT', target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels );

return
