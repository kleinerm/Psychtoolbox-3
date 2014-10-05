function glMultiTexCoord1dARB( target, s )

% glMultiTexCoord1dARB  Interface to OpenGL function glMultiTexCoord1dARB
%
% usage:  glMultiTexCoord1dARB( target, s )
%
% C function:  void glMultiTexCoord1dARB(GLenum target, GLdouble s)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord1dARB', target, s );

return
