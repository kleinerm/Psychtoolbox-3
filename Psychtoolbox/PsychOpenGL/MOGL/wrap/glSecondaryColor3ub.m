function glSecondaryColor3ub( red, green, blue )

% glSecondaryColor3ub  Interface to OpenGL function glSecondaryColor3ub
%
% usage:  glSecondaryColor3ub( red, green, blue )
%
% C function:  void glSecondaryColor3ub(GLubyte red, GLubyte green, GLubyte blue)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3ub', red, green, blue );

return
