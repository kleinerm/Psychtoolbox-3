function glDepthBoundsEXT( zmin, zmax )

% glDepthBoundsEXT  Interface to OpenGL function glDepthBoundsEXT
%
% usage:  glDepthBoundsEXT( zmin, zmax )
%
% C function:  void glDepthBoundsEXT(GLclampd zmin, GLclampd zmax)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDepthBoundsEXT', zmin, zmax );

return
