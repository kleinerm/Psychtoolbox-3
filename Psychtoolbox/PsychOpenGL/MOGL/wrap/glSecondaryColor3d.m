function glSecondaryColor3d( red, green, blue )

% glSecondaryColor3d  Interface to OpenGL function glSecondaryColor3d
%
% usage:  glSecondaryColor3d( red, green, blue )
%
% C function:  void glSecondaryColor3d(GLdouble red, GLdouble green, GLdouble blue)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3d', red, green, blue );

return
