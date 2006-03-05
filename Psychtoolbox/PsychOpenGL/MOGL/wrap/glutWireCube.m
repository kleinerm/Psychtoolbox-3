function glutWireCube( size )

% glutWireCube  Interface to OpenGL function glutWireCube
%
% usage:  glutWireCube( size )
%
% C function:  void glutWireCube(GLdouble size)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glutWireCube', size );

return
