function glSecondaryColor3ui( red, green, blue )

% glSecondaryColor3ui  Interface to OpenGL function glSecondaryColor3ui
%
% usage:  glSecondaryColor3ui( red, green, blue )
%
% C function:  void glSecondaryColor3ui(GLuint red, GLuint green, GLuint blue)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3ui', red, green, blue );

return
