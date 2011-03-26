function glWindowPos2sv( v )

% glWindowPos2sv  Interface to OpenGL function glWindowPos2sv
%
% usage:  glWindowPos2sv( v )
%
% C function:  void glWindowPos2sv(const GLshort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2sv', int16(v) );

return
