function glMultiTexCoord1iARB( target, s )

% glMultiTexCoord1iARB  Interface to OpenGL function glMultiTexCoord1iARB
%
% usage:  glMultiTexCoord1iARB( target, s )
%
% C function:  void glMultiTexCoord1iARB(GLenum target, GLint s)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord1iARB', target, s );

return
