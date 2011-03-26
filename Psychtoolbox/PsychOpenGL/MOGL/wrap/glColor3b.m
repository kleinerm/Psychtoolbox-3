function glColor3b( red, green, blue )

% glColor3b  Interface to OpenGL function glColor3b
%
% usage:  glColor3b( red, green, blue )
%
% C function:  void glColor3b(GLbyte red, GLbyte green, GLbyte blue)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glColor3b', red, green, blue );

return
