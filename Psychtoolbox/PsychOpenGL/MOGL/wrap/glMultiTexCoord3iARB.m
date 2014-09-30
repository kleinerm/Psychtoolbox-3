function glMultiTexCoord3iARB( target, s, t, r )

% glMultiTexCoord3iARB  Interface to OpenGL function glMultiTexCoord3iARB
%
% usage:  glMultiTexCoord3iARB( target, s, t, r )
%
% C function:  void glMultiTexCoord3iARB(GLenum target, GLint s, GLint t, GLint r)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3iARB', target, s, t, r );

return
