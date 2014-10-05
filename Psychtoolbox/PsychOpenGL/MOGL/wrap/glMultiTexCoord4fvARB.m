function glMultiTexCoord4fvARB( target, v )

% glMultiTexCoord4fvARB  Interface to OpenGL function glMultiTexCoord4fvARB
%
% usage:  glMultiTexCoord4fvARB( target, v )
%
% C function:  void glMultiTexCoord4fvARB(GLenum target, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord4fvARB', target, single(v) );

return
