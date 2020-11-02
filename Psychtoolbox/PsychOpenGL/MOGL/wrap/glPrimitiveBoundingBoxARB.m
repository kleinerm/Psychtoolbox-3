function glPrimitiveBoundingBoxARB( minX, minY, minZ, minW, maxX, maxY, maxZ, maxW )

% glPrimitiveBoundingBoxARB  Interface to OpenGL function glPrimitiveBoundingBoxARB
%
% usage:  glPrimitiveBoundingBoxARB( minX, minY, minZ, minW, maxX, maxY, maxZ, maxW )
%
% C function:  void glPrimitiveBoundingBoxARB(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLfloat maxW)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glPrimitiveBoundingBoxARB', minX, minY, minZ, minW, maxX, maxY, maxZ, maxW );

return
