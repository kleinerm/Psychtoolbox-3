function glTexCoord3s( s, t, r )

% glTexCoord3s  Interface to OpenGL function glTexCoord3s
%
% usage:  glTexCoord3s( s, t, r )
%
% C function:  void glTexCoord3s(GLshort s, GLshort t, GLshort r)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord3s', s, t, r );

return
