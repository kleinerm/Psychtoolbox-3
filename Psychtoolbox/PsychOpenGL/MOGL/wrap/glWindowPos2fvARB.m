function glWindowPos2fvARB( v )

% glWindowPos2fvARB  Interface to OpenGL function glWindowPos2fvARB
%
% usage:  glWindowPos2fvARB( v )
%
% C function:  void glWindowPos2fvARB(const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2fvARB', single(v) );

return
