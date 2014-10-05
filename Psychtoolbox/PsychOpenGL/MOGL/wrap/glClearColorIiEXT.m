function glClearColorIiEXT( red, green, blue, alpha )

% glClearColorIiEXT  Interface to OpenGL function glClearColorIiEXT
%
% usage:  glClearColorIiEXT( red, green, blue, alpha )
%
% C function:  void glClearColorIiEXT(GLint red, GLint green, GLint blue, GLint alpha)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glClearColorIiEXT', red, green, blue, alpha );

return
