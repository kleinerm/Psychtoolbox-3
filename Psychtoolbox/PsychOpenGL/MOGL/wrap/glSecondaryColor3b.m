function glSecondaryColor3b( red, green, blue )

% glSecondaryColor3b  Interface to OpenGL function glSecondaryColor3b
%
% usage:  glSecondaryColor3b( red, green, blue )
%
% C function:  void glSecondaryColor3b(GLbyte red, GLbyte green, GLbyte blue)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3b', red, green, blue );

return
