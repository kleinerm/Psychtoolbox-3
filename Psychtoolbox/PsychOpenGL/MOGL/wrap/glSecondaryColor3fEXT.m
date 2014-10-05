function glSecondaryColor3fEXT( red, green, blue )

% glSecondaryColor3fEXT  Interface to OpenGL function glSecondaryColor3fEXT
%
% usage:  glSecondaryColor3fEXT( red, green, blue )
%
% C function:  void glSecondaryColor3fEXT(GLfloat red, GLfloat green, GLfloat blue)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3fEXT', red, green, blue );

return
