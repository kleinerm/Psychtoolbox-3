function glDepthBoundsdNV( zmin, zmax )

% glDepthBoundsdNV  Interface to OpenGL function glDepthBoundsdNV
%
% usage:  glDepthBoundsdNV( zmin, zmax )
%
% C function:  void glDepthBoundsdNV(GLdouble zmin, GLdouble zmax)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDepthBoundsdNV', zmin, zmax );

return
