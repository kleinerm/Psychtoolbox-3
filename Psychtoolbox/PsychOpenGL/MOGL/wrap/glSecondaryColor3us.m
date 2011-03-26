function glSecondaryColor3us( red, green, blue )

% glSecondaryColor3us  Interface to OpenGL function glSecondaryColor3us
%
% usage:  glSecondaryColor3us( red, green, blue )
%
% C function:  void glSecondaryColor3us(GLushort red, GLushort green, GLushort blue)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3us', red, green, blue );

return
