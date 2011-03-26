function glutWireTeapot( size )

% glutWireTeapot  Interface to OpenGL function glutWireTeapot
%
% usage:  glutWireTeapot( size )
%
% C function:  void glutWireTeapot(GLdouble size)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glutWireTeapot', size );

return
