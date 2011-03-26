function glTexCoord4s( s, t, r, q )

% glTexCoord4s  Interface to OpenGL function glTexCoord4s
%
% usage:  glTexCoord4s( s, t, r, q )
%
% C function:  void glTexCoord4s(GLshort s, GLshort t, GLshort r, GLshort q)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord4s', s, t, r, q );

return
