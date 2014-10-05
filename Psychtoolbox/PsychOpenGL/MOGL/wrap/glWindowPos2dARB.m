function glWindowPos2dARB( x, y )

% glWindowPos2dARB  Interface to OpenGL function glWindowPos2dARB
%
% usage:  glWindowPos2dARB( x, y )
%
% C function:  void glWindowPos2dARB(GLdouble x, GLdouble y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2dARB', x, y );

return
