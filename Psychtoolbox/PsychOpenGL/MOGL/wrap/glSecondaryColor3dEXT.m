function glSecondaryColor3dEXT( red, green, blue )

% glSecondaryColor3dEXT  Interface to OpenGL function glSecondaryColor3dEXT
%
% usage:  glSecondaryColor3dEXT( red, green, blue )
%
% C function:  void glSecondaryColor3dEXT(GLdouble red, GLdouble green, GLdouble blue)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3dEXT', red, green, blue );

return
