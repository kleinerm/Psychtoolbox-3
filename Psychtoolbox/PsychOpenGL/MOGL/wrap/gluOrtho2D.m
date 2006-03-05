function gluOrtho2D( left, right, bottom, top )

% gluOrtho2D  Interface to OpenGL function gluOrtho2D
%
% usage:  gluOrtho2D( left, right, bottom, top )
%
% C function:  void gluOrtho2D(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'gluOrtho2D', left, right, bottom, top );

return
