function glTexCoord2f( s, t )

% glTexCoord2f  Interface to OpenGL function glTexCoord2f
%
% usage:  glTexCoord2f( s, t )
%
% C function:  void glTexCoord2f(GLfloat s, GLfloat t)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2f', s, t );

return
