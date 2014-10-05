function glMultiTexSubImage2DEXT( texunit, target, level, xoffset, yoffset, width, height, format, type, pixels )

% glMultiTexSubImage2DEXT  Interface to OpenGL function glMultiTexSubImage2DEXT
%
% usage:  glMultiTexSubImage2DEXT( texunit, target, level, xoffset, yoffset, width, height, format, type, pixels )
%
% C function:  void glMultiTexSubImage2DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=10,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexSubImage2DEXT', texunit, target, level, xoffset, yoffset, width, height, format, type, pixels );

return
