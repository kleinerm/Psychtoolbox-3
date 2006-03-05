function glutWireTeapot( size )

% glutWireTeapot  Interface to OpenGL function glutWireTeapot
%
% usage:  glutWireTeapot( size )
%
% C function:  void glutWireTeapot(GLdouble size)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glutWireTeapot', size );

return
