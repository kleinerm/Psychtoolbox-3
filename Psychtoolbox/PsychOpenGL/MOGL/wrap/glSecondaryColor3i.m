function glSecondaryColor3i( red, green, blue )

% glSecondaryColor3i  Interface to OpenGL function glSecondaryColor3i
%
% usage:  glSecondaryColor3i( red, green, blue )
%
% C function:  void glSecondaryColor3i(GLint red, GLint green, GLint blue)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3i', red, green, blue );

return
