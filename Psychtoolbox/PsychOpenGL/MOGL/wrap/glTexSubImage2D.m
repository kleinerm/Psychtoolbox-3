function glTexSubImage2D( target, level, xoffset, yoffset, width, height, format, type, pixels )

% glTexSubImage2D  Interface to OpenGL function glTexSubImage2D
%
% usage:  glTexSubImage2D( target, level, xoffset, yoffset, width, height, format, type, pixels )
%
% C function:  void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glTexSubImage2D', target, level, xoffset, yoffset, width, height, format, type, pixels );

return
