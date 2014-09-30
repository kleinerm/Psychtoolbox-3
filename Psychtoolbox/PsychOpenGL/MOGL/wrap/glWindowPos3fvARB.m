function glWindowPos3fvARB( v )

% glWindowPos3fvARB  Interface to OpenGL function glWindowPos3fvARB
%
% usage:  glWindowPos3fvARB( v )
%
% C function:  void glWindowPos3fvARB(const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3fvARB', single(v) );

return
