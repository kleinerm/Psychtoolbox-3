function glDepthRangedNV( zNear, zFar )

% glDepthRangedNV  Interface to OpenGL function glDepthRangedNV
%
% usage:  glDepthRangedNV( zNear, zFar )
%
% C function:  void glDepthRangedNV(GLdouble zNear, GLdouble zFar)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDepthRangedNV', zNear, zFar );

return
