function glSecondaryColor3iEXT( red, green, blue )

% glSecondaryColor3iEXT  Interface to OpenGL function glSecondaryColor3iEXT
%
% usage:  glSecondaryColor3iEXT( red, green, blue )
%
% C function:  void glSecondaryColor3iEXT(GLint red, GLint green, GLint blue)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3iEXT', red, green, blue );

return
