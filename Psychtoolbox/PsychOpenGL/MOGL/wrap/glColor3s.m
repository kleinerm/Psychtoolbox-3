function glColor3s( red, green, blue )

% glColor3s  Interface to OpenGL function glColor3s
%
% usage:  glColor3s( red, green, blue )
%
% C function:  void glColor3s(GLshort red, GLshort green, GLshort blue)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glColor3s', red, green, blue );

return
