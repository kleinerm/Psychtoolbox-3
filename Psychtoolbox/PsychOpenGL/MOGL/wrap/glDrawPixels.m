function glDrawPixels( width, height, format, type, pixels )

% glDrawPixels  Interface to OpenGL function glDrawPixels
%
% usage:  glDrawPixels( width, height, format, type, pixels )
%
% C function:  void glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glDrawPixels', width, height, format, type, pixels );

return
