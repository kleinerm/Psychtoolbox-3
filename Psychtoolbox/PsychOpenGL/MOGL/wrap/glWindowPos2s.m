function glWindowPos2s( x, y )

% glWindowPos2s  Interface to OpenGL function glWindowPos2s
%
% usage:  glWindowPos2s( x, y )
%
% C function:  void glWindowPos2s(GLshort x, GLshort y)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2s', x, y );

return
