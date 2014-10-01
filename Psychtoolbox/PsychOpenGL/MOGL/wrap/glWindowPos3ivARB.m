function glWindowPos3ivARB( v )

% glWindowPos3ivARB  Interface to OpenGL function glWindowPos3ivARB
%
% usage:  glWindowPos3ivARB( v )
%
% C function:  void glWindowPos3ivARB(const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3ivARB', int32(v) );

return
