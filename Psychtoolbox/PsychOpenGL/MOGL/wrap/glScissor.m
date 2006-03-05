function glScissor( x, y, width, height )

% glScissor  Interface to OpenGL function glScissor
%
% usage:  glScissor( x, y, width, height )
%
% C function:  void glScissor(GLint x, GLint y, GLsizei width, GLsizei height)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glScissor', x, y, width, height );

return
