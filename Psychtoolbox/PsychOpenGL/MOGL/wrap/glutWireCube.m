function glutWireCube( size )

% glutWireCube  Interface to OpenGL function glutWireCube
%
% usage:  glutWireCube( size )
%
% C function:  void glutWireCube(GLdouble size)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glutWireCube', size );

return
