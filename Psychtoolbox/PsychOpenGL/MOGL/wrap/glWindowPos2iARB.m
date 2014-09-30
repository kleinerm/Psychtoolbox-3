function glWindowPos2iARB( x, y )

% glWindowPos2iARB  Interface to OpenGL function glWindowPos2iARB
%
% usage:  glWindowPos2iARB( x, y )
%
% C function:  void glWindowPos2iARB(GLint x, GLint y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2iARB', x, y );

return
