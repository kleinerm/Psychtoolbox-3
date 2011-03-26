function glTexCoord3i( s, t, r )

% glTexCoord3i  Interface to OpenGL function glTexCoord3i
%
% usage:  glTexCoord3i( s, t, r )
%
% C function:  void glTexCoord3i(GLint s, GLint t, GLint r)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord3i', s, t, r );

return
