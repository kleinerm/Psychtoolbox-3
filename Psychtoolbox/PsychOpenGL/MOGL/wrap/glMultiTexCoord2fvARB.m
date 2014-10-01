function glMultiTexCoord2fvARB( target, v )

% glMultiTexCoord2fvARB  Interface to OpenGL function glMultiTexCoord2fvARB
%
% usage:  glMultiTexCoord2fvARB( target, v )
%
% C function:  void glMultiTexCoord2fvARB(GLenum target, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord2fvARB', target, single(v) );

return
