function glOrtho( left, right, bottom, top, zNear, zFar )

% glOrtho  Interface to OpenGL function glOrtho
%
% usage:  glOrtho( left, right, bottom, top, zNear, zFar )
%
% C function:  void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glOrtho', left, right, bottom, top, zNear, zFar );

return
