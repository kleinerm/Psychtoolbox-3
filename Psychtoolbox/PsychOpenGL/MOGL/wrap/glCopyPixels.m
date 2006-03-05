function glCopyPixels( x, y, width, height, type )

% glCopyPixels  Interface to OpenGL function glCopyPixels
%
% usage:  glCopyPixels( x, y, width, height, type )
%
% C function:  void glCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glCopyPixels', x, y, width, height, type );

return
