function glOrthofOES( l, r, b, t, n, f )

% glOrthofOES  Interface to OpenGL function glOrthofOES
%
% usage:  glOrthofOES( l, r, b, t, n, f )
%
% C function:  void glOrthofOES(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glOrthofOES', l, r, b, t, n, f );

return
