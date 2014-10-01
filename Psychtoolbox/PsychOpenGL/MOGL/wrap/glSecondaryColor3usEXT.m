function glSecondaryColor3usEXT( red, green, blue )

% glSecondaryColor3usEXT  Interface to OpenGL function glSecondaryColor3usEXT
%
% usage:  glSecondaryColor3usEXT( red, green, blue )
%
% C function:  void glSecondaryColor3usEXT(GLushort red, GLushort green, GLushort blue)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3usEXT', red, green, blue );

return
