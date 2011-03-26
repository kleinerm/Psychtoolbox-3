function glTexCoord4i( s, t, r, q )

% glTexCoord4i  Interface to OpenGL function glTexCoord4i
%
% usage:  glTexCoord4i( s, t, r, q )
%
% C function:  void glTexCoord4i(GLint s, GLint t, GLint r, GLint q)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord4i', s, t, r, q );

return
