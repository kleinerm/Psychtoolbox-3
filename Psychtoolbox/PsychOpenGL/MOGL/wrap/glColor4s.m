function glColor4s( red, green, blue, alpha )

% glColor4s  Interface to OpenGL function glColor4s
%
% usage:  glColor4s( red, green, blue, alpha )
%
% C function:  void glColor4s(GLshort red, GLshort green, GLshort blue, GLshort alpha)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glColor4s', red, green, blue, alpha );

return
