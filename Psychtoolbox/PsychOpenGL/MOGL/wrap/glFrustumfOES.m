function glFrustumfOES( l, r, b, t, n, f )

% glFrustumfOES  Interface to OpenGL function glFrustumfOES
%
% usage:  glFrustumfOES( l, r, b, t, n, f )
%
% C function:  void glFrustumfOES(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glFrustumfOES', l, r, b, t, n, f );

return
