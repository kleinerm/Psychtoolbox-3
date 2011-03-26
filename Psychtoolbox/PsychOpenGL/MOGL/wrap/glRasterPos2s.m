function glRasterPos2s( x, y )

% glRasterPos2s  Interface to OpenGL function glRasterPos2s
%
% usage:  glRasterPos2s( x, y )
%
% C function:  void glRasterPos2s(GLshort x, GLshort y)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos2s', x, y );

return
