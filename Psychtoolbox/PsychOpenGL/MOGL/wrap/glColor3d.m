function glColor3d( red, green, blue )

% glColor3d  Interface to OpenGL function glColor3d
%
% usage:  glColor3d( red, green, blue )
%
% C function:  void glColor3d(GLdouble red, GLdouble green, GLdouble blue)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glColor3d', red, green, blue );

return
