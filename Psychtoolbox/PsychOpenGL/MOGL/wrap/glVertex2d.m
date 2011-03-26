function glVertex2d( x, y )

% glVertex2d  Interface to OpenGL function glVertex2d
%
% usage:  glVertex2d( x, y )
%
% C function:  void glVertex2d(GLdouble x, GLdouble y)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertex2d', x, y );

return
