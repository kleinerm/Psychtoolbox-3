function glSecondaryColor3sEXT( red, green, blue )

% glSecondaryColor3sEXT  Interface to OpenGL function glSecondaryColor3sEXT
%
% usage:  glSecondaryColor3sEXT( red, green, blue )
%
% C function:  void glSecondaryColor3sEXT(GLshort red, GLshort green, GLshort blue)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3sEXT', red, green, blue );

return
