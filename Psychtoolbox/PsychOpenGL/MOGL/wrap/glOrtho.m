function glOrtho( left, right, bottom, top, zNear, zFar )

% glOrtho  Interface to OpenGL function glOrtho
%
% usage:  glOrtho( left, right, bottom, top, zNear, zFar )
%
% C function:  void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glOrtho', left, right, bottom, top, zNear, zFar );

return
