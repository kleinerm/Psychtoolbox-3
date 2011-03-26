function glTexCoord2sv( v )

% glTexCoord2sv  Interface to OpenGL function glTexCoord2sv
%
% usage:  glTexCoord2sv( v )
%
% C function:  void glTexCoord2sv(const GLshort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2sv', int16(v) );

return
