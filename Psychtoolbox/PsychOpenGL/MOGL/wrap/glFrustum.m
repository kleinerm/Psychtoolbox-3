function glFrustum( left, right, bottom, top, zNear, zFar )

% glFrustum  Interface to OpenGL function glFrustum
%
% usage:  glFrustum( left, right, bottom, top, zNear, zFar )
%
% C function:  void glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glFrustum', left, right, bottom, top, zNear, zFar );

return
