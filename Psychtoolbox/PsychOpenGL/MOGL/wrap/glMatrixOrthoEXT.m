function glMatrixOrthoEXT( mode, left, right, bottom, top, zNear, zFar )

% glMatrixOrthoEXT  Interface to OpenGL function glMatrixOrthoEXT
%
% usage:  glMatrixOrthoEXT( mode, left, right, bottom, top, zNear, zFar )
%
% C function:  void glMatrixOrthoEXT(GLenum mode, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glMatrixOrthoEXT', mode, left, right, bottom, top, zNear, zFar );

return
