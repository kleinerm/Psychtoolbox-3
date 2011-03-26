function glColor3ub( red, green, blue )

% glColor3ub  Interface to OpenGL function glColor3ub
%
% usage:  glColor3ub( red, green, blue )
%
% C function:  void glColor3ub(GLubyte red, GLubyte green, GLubyte blue)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glColor3ub', red, green, blue );

return
