function glMultiTexCoord2iARB( target, s, t )

% glMultiTexCoord2iARB  Interface to OpenGL function glMultiTexCoord2iARB
%
% usage:  glMultiTexCoord2iARB( target, s, t )
%
% C function:  void glMultiTexCoord2iARB(GLenum target, GLint s, GLint t)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord2iARB', target, s, t );

return
