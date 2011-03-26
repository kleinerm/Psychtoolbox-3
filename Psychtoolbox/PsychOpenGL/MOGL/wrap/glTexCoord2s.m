function glTexCoord2s( s, t )

% glTexCoord2s  Interface to OpenGL function glTexCoord2s
%
% usage:  glTexCoord2s( s, t )
%
% C function:  void glTexCoord2s(GLshort s, GLshort t)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2s', s, t );

return
