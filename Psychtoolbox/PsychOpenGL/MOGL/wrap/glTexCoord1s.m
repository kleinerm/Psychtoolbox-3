function glTexCoord1s( s )

% glTexCoord1s  Interface to OpenGL function glTexCoord1s
%
% usage:  glTexCoord1s( s )
%
% C function:  void glTexCoord1s(GLshort s)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord1s', s );

return
