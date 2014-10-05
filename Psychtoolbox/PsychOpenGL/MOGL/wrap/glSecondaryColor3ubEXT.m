function glSecondaryColor3ubEXT( red, green, blue )

% glSecondaryColor3ubEXT  Interface to OpenGL function glSecondaryColor3ubEXT
%
% usage:  glSecondaryColor3ubEXT( red, green, blue )
%
% C function:  void glSecondaryColor3ubEXT(GLubyte red, GLubyte green, GLubyte blue)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3ubEXT', red, green, blue );

return
