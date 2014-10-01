function glMultiTexCoord3fvARB( target, v )

% glMultiTexCoord3fvARB  Interface to OpenGL function glMultiTexCoord3fvARB
%
% usage:  glMultiTexCoord3fvARB( target, v )
%
% C function:  void glMultiTexCoord3fvARB(GLenum target, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3fvARB', target, single(v) );

return
