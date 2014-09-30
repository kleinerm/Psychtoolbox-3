function glClearColorIuiEXT( red, green, blue, alpha )

% glClearColorIuiEXT  Interface to OpenGL function glClearColorIuiEXT
%
% usage:  glClearColorIuiEXT( red, green, blue, alpha )
%
% C function:  void glClearColorIuiEXT(GLuint red, GLuint green, GLuint blue, GLuint alpha)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glClearColorIuiEXT', red, green, blue, alpha );

return
