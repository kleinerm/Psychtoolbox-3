function glMultiTexSubImage3DEXT( texunit, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels )

% glMultiTexSubImage3DEXT  Interface to OpenGL function glMultiTexSubImage3DEXT
%
% usage:  glMultiTexSubImage3DEXT( texunit, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels )
%
% C function:  void glMultiTexSubImage3DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=12,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexSubImage3DEXT', texunit, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels );

return
