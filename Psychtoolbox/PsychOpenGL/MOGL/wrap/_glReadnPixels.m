function data = glReadnPixels( x, y, width, height, format, type, bufSize )

% glReadnPixels  Interface to OpenGL function glReadnPixels
%
% usage:  data = glReadnPixels( x, y, width, height, format, type, bufSize )
%
% C function:  void glReadnPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=7,
    error('invalid number of arguments');
end

data = (0);

moglcore( 'glReadnPixels', x, y, width, height, format, type, bufSize, data );

return
