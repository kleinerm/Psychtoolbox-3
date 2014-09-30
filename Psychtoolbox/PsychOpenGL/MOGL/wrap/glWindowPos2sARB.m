function glWindowPos2sARB( x, y )

% glWindowPos2sARB  Interface to OpenGL function glWindowPos2sARB
%
% usage:  glWindowPos2sARB( x, y )
%
% C function:  void glWindowPos2sARB(GLshort x, GLshort y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2sARB', x, y );

return
