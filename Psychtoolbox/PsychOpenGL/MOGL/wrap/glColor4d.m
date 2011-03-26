function glColor4d( red, green, blue, alpha )

% glColor4d  Interface to OpenGL function glColor4d
%
% usage:  glColor4d( red, green, blue, alpha )
%
% C function:  void glColor4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glColor4d', red, green, blue, alpha );

return
