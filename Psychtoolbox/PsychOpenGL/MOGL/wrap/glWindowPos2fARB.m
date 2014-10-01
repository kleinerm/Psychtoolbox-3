function glWindowPos2fARB( x, y )

% glWindowPos2fARB  Interface to OpenGL function glWindowPos2fARB
%
% usage:  glWindowPos2fARB( x, y )
%
% C function:  void glWindowPos2fARB(GLfloat x, GLfloat y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2fARB', x, y );

return
