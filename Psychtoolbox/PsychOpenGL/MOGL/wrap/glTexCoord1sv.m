function glTexCoord1sv( v )

% glTexCoord1sv  Interface to OpenGL function glTexCoord1sv
%
% usage:  glTexCoord1sv( v )
%
% C function:  void glTexCoord1sv(const GLshort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord1sv', int16(v) );

return
