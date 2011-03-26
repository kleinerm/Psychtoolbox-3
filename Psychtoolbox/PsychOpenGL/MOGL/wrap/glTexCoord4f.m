function glTexCoord4f( s, t, r, q )

% glTexCoord4f  Interface to OpenGL function glTexCoord4f
%
% usage:  glTexCoord4f( s, t, r, q )
%
% C function:  void glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord4f', s, t, r, q );

return
