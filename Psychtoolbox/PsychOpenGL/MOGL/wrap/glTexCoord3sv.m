function glTexCoord3sv( v )

% glTexCoord3sv  Interface to OpenGL function glTexCoord3sv
%
% usage:  glTexCoord3sv( v )
%
% C function:  void glTexCoord3sv(const GLshort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord3sv', int16(v) );

return
