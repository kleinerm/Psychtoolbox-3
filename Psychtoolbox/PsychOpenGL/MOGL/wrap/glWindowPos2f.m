function glWindowPos2f( x, y )

% glWindowPos2f  Interface to OpenGL function glWindowPos2f
%
% usage:  glWindowPos2f( x, y )
%
% C function:  void glWindowPos2f(GLfloat x, GLfloat y)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2f', x, y );

return
