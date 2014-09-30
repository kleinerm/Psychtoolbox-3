function glMultiTexCoord1fARB( target, s )

% glMultiTexCoord1fARB  Interface to OpenGL function glMultiTexCoord1fARB
%
% usage:  glMultiTexCoord1fARB( target, s )
%
% C function:  void glMultiTexCoord1fARB(GLenum target, GLfloat s)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord1fARB', target, s );

return
