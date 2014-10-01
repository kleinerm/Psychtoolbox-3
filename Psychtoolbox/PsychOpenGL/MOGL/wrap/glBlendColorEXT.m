function glBlendColorEXT( red, green, blue, alpha )

% glBlendColorEXT  Interface to OpenGL function glBlendColorEXT
%
% usage:  glBlendColorEXT( red, green, blue, alpha )
%
% C function:  void glBlendColorEXT(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBlendColorEXT', red, green, blue, alpha );

return
