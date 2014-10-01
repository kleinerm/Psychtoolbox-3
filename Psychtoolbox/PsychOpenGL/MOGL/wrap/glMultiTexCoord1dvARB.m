function glMultiTexCoord1dvARB( target, v )

% glMultiTexCoord1dvARB  Interface to OpenGL function glMultiTexCoord1dvARB
%
% usage:  glMultiTexCoord1dvARB( target, v )
%
% C function:  void glMultiTexCoord1dvARB(GLenum target, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord1dvARB', target, double(v) );

return
