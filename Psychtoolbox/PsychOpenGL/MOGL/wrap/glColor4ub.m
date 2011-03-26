function glColor4ub( red, green, blue, alpha )

% glColor4ub  Interface to OpenGL function glColor4ub
%
% usage:  glColor4ub( red, green, blue, alpha )
%
% C function:  void glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glColor4ub', red, green, blue, alpha );

return
