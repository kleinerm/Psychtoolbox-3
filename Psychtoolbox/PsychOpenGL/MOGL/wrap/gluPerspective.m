function gluPerspective( fovy, aspect, zNear, zFar )

% gluPerspective  Interface to OpenGL function gluPerspective
%
% usage:  gluPerspective( fovy, aspect, zNear, zFar )
%
% C function:  void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'gluPerspective', fovy, aspect, zNear, zFar );

return
