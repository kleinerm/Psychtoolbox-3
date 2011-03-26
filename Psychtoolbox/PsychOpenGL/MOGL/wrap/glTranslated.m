function glTranslated( x, y, z )

% glTranslated  Interface to OpenGL function glTranslated
%
% usage:  glTranslated( x, y, z )
%
% C function:  void glTranslated(GLdouble x, GLdouble y, GLdouble z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTranslated', x, y, z );

return
