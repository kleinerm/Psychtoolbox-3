function glSecondaryColor3f( red, green, blue )

% glSecondaryColor3f  Interface to OpenGL function glSecondaryColor3f
%
% usage:  glSecondaryColor3f( red, green, blue )
%
% C function:  void glSecondaryColor3f(GLfloat red, GLfloat green, GLfloat blue)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3f', red, green, blue );

return
