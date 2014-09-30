function glWindowPos2svARB( v )

% glWindowPos2svARB  Interface to OpenGL function glWindowPos2svARB
%
% usage:  glWindowPos2svARB( v )
%
% C function:  void glWindowPos2svARB(const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2svARB', int16(v) );

return
