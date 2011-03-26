function glScissor( x, y, width, height )

% glScissor  Interface to OpenGL function glScissor
%
% usage:  glScissor( x, y, width, height )
%
% C function:  void glScissor(GLint x, GLint y, GLsizei width, GLsizei height)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glScissor', x, y, width, height );

return
