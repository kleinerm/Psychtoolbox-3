function glClearStencil( s )

% glClearStencil  Interface to OpenGL function glClearStencil
%
% usage:  glClearStencil( s )
%
% C function:  void glClearStencil(GLint s)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glClearStencil', s );

return
