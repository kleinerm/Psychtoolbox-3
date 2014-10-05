function glTexSubImage2DEXT( target, level, xoffset, yoffset, width, height, format, type, pixels )

% glTexSubImage2DEXT  Interface to OpenGL function glTexSubImage2DEXT
%
% usage:  glTexSubImage2DEXT( target, level, xoffset, yoffset, width, height, format, type, pixels )
%
% C function:  void glTexSubImage2DEXT(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glTexSubImage2DEXT', target, level, xoffset, yoffset, width, height, format, type, pixels );

return
