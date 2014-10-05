function glMultiTexCoord4iARB( target, s, t, r, q )

% glMultiTexCoord4iARB  Interface to OpenGL function glMultiTexCoord4iARB
%
% usage:  glMultiTexCoord4iARB( target, s, t, r, q )
%
% C function:  void glMultiTexCoord4iARB(GLenum target, GLint s, GLint t, GLint r, GLint q)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord4iARB', target, s, t, r, q );

return
