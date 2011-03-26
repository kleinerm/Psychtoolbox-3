function glColor4us( red, green, blue, alpha )

% glColor4us  Interface to OpenGL function glColor4us
%
% usage:  glColor4us( red, green, blue, alpha )
%
% C function:  void glColor4us(GLushort red, GLushort green, GLushort blue, GLushort alpha)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glColor4us', red, green, blue, alpha );

return
