function glColor3ui( red, green, blue )

% glColor3ui  Interface to OpenGL function glColor3ui
%
% usage:  glColor3ui( red, green, blue )
%
% C function:  void glColor3ui(GLuint red, GLuint green, GLuint blue)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glColor3ui', red, green, blue );

return
