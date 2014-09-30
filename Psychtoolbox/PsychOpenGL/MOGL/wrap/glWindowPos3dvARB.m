function glWindowPos3dvARB( v )

% glWindowPos3dvARB  Interface to OpenGL function glWindowPos3dvARB
%
% usage:  glWindowPos3dvARB( v )
%
% C function:  void glWindowPos3dvARB(const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3dvARB', double(v) );

return
