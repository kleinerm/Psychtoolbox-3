function glMultiTexCoord1fvARB( target, v )

% glMultiTexCoord1fvARB  Interface to OpenGL function glMultiTexCoord1fvARB
%
% usage:  glMultiTexCoord1fvARB( target, v )
%
% C function:  void glMultiTexCoord1fvARB(GLenum target, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord1fvARB', target, single(v) );

return
