function glutSolidCube( size )

% glutSolidCube  Interface to OpenGL function glutSolidCube
%
% usage:  glutSolidCube( size )
%
% C function:  void glutSolidCube(GLdouble size)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glutSolidCube', size );

return
