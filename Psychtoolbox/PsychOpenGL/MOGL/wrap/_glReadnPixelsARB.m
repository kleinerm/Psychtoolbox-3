function data = glReadnPixelsARB( x, y, width, height, format, type, bufSize )

% glReadnPixelsARB  Interface to OpenGL function glReadnPixelsARB
%
% usage:  data = glReadnPixelsARB( x, y, width, height, format, type, bufSize )
%
% C function:  void glReadnPixelsARB(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, GLvoid* data)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---

if nargin~=7,
    error('invalid number of arguments');
end

data = (0);

moglcore( 'glReadnPixelsARB', x, y, width, height, format, type, bufSize, data );

return
