function glSecondaryColor3bEXT( red, green, blue )

% glSecondaryColor3bEXT  Interface to OpenGL function glSecondaryColor3bEXT
%
% usage:  glSecondaryColor3bEXT( red, green, blue )
%
% C function:  void glSecondaryColor3bEXT(GLbyte red, GLbyte green, GLbyte blue)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3bEXT', red, green, blue );

return
