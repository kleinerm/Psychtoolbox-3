function glClearAccum( red, green, blue, alpha )

% glClearAccum  Interface to OpenGL function glClearAccum
%
% usage:  glClearAccum( red, green, blue, alpha )
%
% C function:  void glClearAccum(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glClearAccum', red, green, blue, alpha );

return
