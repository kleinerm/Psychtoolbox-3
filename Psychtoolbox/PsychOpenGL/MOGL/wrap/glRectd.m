function glRectd( x1, y1, x2, y2 )

% glRectd  Interface to OpenGL function glRectd
%
% usage:  glRectd( x1, y1, x2, y2 )
%
% C function:  void glRectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glRectd', x1, y1, x2, y2 );

return
