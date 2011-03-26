function glTexCoord3f( s, t, r )

% glTexCoord3f  Interface to OpenGL function glTexCoord3f
%
% usage:  glTexCoord3f( s, t, r )
%
% C function:  void glTexCoord3f(GLfloat s, GLfloat t, GLfloat r)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord3f', s, t, r );

return
