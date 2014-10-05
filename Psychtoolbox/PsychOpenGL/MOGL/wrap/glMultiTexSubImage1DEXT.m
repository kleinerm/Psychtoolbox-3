function glMultiTexSubImage1DEXT( texunit, target, level, xoffset, width, format, type, pixels )

% glMultiTexSubImage1DEXT  Interface to OpenGL function glMultiTexSubImage1DEXT
%
% usage:  glMultiTexSubImage1DEXT( texunit, target, level, xoffset, width, format, type, pixels )
%
% C function:  void glMultiTexSubImage1DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexSubImage1DEXT', texunit, target, level, xoffset, width, format, type, pixels );

return
