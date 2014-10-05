function glWindowPos3svARB( v )

% glWindowPos3svARB  Interface to OpenGL function glWindowPos3svARB
%
% usage:  glWindowPos3svARB( v )
%
% C function:  void glWindowPos3svARB(const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3svARB', int16(v) );

return
