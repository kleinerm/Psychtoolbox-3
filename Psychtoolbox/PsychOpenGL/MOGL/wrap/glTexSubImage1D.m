function glTexSubImage1D( target, level, xoffset, width, format, type, pixels )

% glTexSubImage1D  Interface to OpenGL function glTexSubImage1D
%
% usage:  glTexSubImage1D( target, level, xoffset, width, format, type, pixels )
%
% C function:  void glTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid* pixels)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glTexSubImage1D', target, level, xoffset, width, format, type, pixels );

return
