function glWindowPos2dvARB( v )

% glWindowPos2dvARB  Interface to OpenGL function glWindowPos2dvARB
%
% usage:  glWindowPos2dvARB( v )
%
% C function:  void glWindowPos2dvARB(const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2dvARB', double(v) );

return
