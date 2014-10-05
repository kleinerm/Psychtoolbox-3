function glTexSubImage1DEXT( target, level, xoffset, width, format, type, pixels )

% glTexSubImage1DEXT  Interface to OpenGL function glTexSubImage1DEXT
%
% usage:  glTexSubImage1DEXT( target, level, xoffset, width, format, type, pixels )
%
% C function:  void glTexSubImage1DEXT(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glTexSubImage1DEXT', target, level, xoffset, width, format, type, pixels );

return
