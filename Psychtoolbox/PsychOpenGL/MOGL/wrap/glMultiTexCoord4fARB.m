function glMultiTexCoord4fARB( target, s, t, r, q )

% glMultiTexCoord4fARB  Interface to OpenGL function glMultiTexCoord4fARB
%
% usage:  glMultiTexCoord4fARB( target, s, t, r, q )
%
% C function:  void glMultiTexCoord4fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord4fARB', target, s, t, r, q );

return
