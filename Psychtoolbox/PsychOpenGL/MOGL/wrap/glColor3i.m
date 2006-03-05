function glColor3i( red, green, blue )

% glColor3i  Interface to OpenGL function glColor3i
%
% usage:  glColor3i( red, green, blue )
%
% C function:  void glColor3i(GLint red, GLint green, GLint blue)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glColor3i', red, green, blue );

return
