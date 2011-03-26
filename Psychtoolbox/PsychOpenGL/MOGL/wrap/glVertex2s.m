function glVertex2s( x, y )

% glVertex2s  Interface to OpenGL function glVertex2s
%
% usage:  glVertex2s( x, y )
%
% C function:  void glVertex2s(GLshort x, GLshort y)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertex2s', x, y );

return
