function glClearTexSubImage( texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data )

% glClearTexSubImage  Interface to OpenGL function glClearTexSubImage
%
% usage:  glClearTexSubImage( texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data )
%
% C function:  void glClearTexSubImage(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=11,
    error('invalid number of arguments');
end

moglcore( 'glClearTexSubImage', texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data );

return
