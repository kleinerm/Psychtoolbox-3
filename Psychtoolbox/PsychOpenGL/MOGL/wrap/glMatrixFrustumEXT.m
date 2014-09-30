function glMatrixFrustumEXT( mode, left, right, bottom, top, zNear, zFar )

% glMatrixFrustumEXT  Interface to OpenGL function glMatrixFrustumEXT
%
% usage:  glMatrixFrustumEXT( mode, left, right, bottom, top, zNear, zFar )
%
% C function:  void glMatrixFrustumEXT(GLenum mode, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glMatrixFrustumEXT', mode, left, right, bottom, top, zNear, zFar );

return
