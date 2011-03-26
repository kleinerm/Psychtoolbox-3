function glTexCoord2i( s, t )

% glTexCoord2i  Interface to OpenGL function glTexCoord2i
%
% usage:  glTexCoord2i( s, t )
%
% C function:  void glTexCoord2i(GLint s, GLint t)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2i', s, t );

return
