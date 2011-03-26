function glColor3f( red, green, blue )

% glColor3f  Interface to OpenGL function glColor3f
%
% usage:  glColor3f( red, green, blue )
%
% C function:  void glColor3f(GLfloat red, GLfloat green, GLfloat blue)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glColor3f', red, green, blue );

return
