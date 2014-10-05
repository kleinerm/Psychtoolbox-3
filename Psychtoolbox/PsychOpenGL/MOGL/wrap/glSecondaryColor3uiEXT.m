function glSecondaryColor3uiEXT( red, green, blue )

% glSecondaryColor3uiEXT  Interface to OpenGL function glSecondaryColor3uiEXT
%
% usage:  glSecondaryColor3uiEXT( red, green, blue )
%
% C function:  void glSecondaryColor3uiEXT(GLuint red, GLuint green, GLuint blue)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3uiEXT', red, green, blue );

return
