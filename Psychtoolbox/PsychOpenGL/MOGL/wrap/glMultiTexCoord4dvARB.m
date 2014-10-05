function glMultiTexCoord4dvARB( target, v )

% glMultiTexCoord4dvARB  Interface to OpenGL function glMultiTexCoord4dvARB
%
% usage:  glMultiTexCoord4dvARB( target, v )
%
% C function:  void glMultiTexCoord4dvARB(GLenum target, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord4dvARB', target, double(v) );

return
