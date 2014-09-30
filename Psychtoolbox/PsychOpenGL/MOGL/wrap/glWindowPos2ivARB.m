function glWindowPos2ivARB( v )

% glWindowPos2ivARB  Interface to OpenGL function glWindowPos2ivARB
%
% usage:  glWindowPos2ivARB( v )
%
% C function:  void glWindowPos2ivARB(const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2ivARB', int32(v) );

return
