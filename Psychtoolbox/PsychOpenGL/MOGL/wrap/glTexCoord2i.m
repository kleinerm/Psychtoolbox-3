function glTexCoord2i( s, t )

% glTexCoord2i  Interface to OpenGL function glTexCoord2i
%
% usage:  glTexCoord2i( s, t )
%
% C function:  void glTexCoord2i(GLint s, GLint t)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2i', s, t );

return
