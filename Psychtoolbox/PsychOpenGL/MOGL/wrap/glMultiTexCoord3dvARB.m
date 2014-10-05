function glMultiTexCoord3dvARB( target, v )

% glMultiTexCoord3dvARB  Interface to OpenGL function glMultiTexCoord3dvARB
%
% usage:  glMultiTexCoord3dvARB( target, v )
%
% C function:  void glMultiTexCoord3dvARB(GLenum target, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3dvARB', target, double(v) );

return
