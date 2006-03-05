function glVertex2i( x, y )

% glVertex2i  Interface to OpenGL function glVertex2i
%
% usage:  glVertex2i( x, y )
%
% C function:  void glVertex2i(GLint x, GLint y)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertex2i', x, y );

return
