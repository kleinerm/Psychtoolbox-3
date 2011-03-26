function glColor4ui( red, green, blue, alpha )

% glColor4ui  Interface to OpenGL function glColor4ui
%
% usage:  glColor4ui( red, green, blue, alpha )
%
% C function:  void glColor4ui(GLuint red, GLuint green, GLuint blue, GLuint alpha)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glColor4ui', red, green, blue, alpha );

return
