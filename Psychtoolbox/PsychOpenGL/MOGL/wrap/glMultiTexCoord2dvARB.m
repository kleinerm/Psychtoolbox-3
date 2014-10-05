function glMultiTexCoord2dvARB( target, v )

% glMultiTexCoord2dvARB  Interface to OpenGL function glMultiTexCoord2dvARB
%
% usage:  glMultiTexCoord2dvARB( target, v )
%
% C function:  void glMultiTexCoord2dvARB(GLenum target, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord2dvARB', target, double(v) );

return
