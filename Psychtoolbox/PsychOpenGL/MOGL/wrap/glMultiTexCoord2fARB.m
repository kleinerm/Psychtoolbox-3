function glMultiTexCoord2fARB( target, s, t )

% glMultiTexCoord2fARB  Interface to OpenGL function glMultiTexCoord2fARB
%
% usage:  glMultiTexCoord2fARB( target, s, t )
%
% C function:  void glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord2fARB', target, s, t );

return
