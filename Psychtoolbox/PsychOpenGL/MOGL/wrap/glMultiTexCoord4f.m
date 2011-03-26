function glMultiTexCoord4f( target, s, t, r, q )

% glMultiTexCoord4f  Interface to OpenGL function glMultiTexCoord4f
%
% usage:  glMultiTexCoord4f( target, s, t, r, q )
%
% C function:  void glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord4f', target, s, t, r, q );

return
