function glColor4f( red, green, blue, alpha )

% glColor4f  Interface to OpenGL function glColor4f
%
% usage:  glColor4f( red, green, blue, alpha )
%
% C function:  void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glColor4f', red, green, blue, alpha );

return
