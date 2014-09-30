function glMultiTexCoord3fARB( target, s, t, r )

% glMultiTexCoord3fARB  Interface to OpenGL function glMultiTexCoord3fARB
%
% usage:  glMultiTexCoord3fARB( target, s, t, r )
%
% C function:  void glMultiTexCoord3fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3fARB', target, s, t, r );

return
