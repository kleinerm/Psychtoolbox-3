function glRects( x1, y1, x2, y2 )

% glRects  Interface to OpenGL function glRects
%
% usage:  glRects( x1, y1, x2, y2 )
%
% C function:  void glRects(GLshort x1, GLshort y1, GLshort x2, GLshort y2)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glRects', x1, y1, x2, y2 );

return
