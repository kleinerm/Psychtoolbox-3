function glMultiTexCoord1sARB( target, s )

% glMultiTexCoord1sARB  Interface to OpenGL function glMultiTexCoord1sARB
%
% usage:  glMultiTexCoord1sARB( target, s )
%
% C function:  void glMultiTexCoord1sARB(GLenum target, GLshort s)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord1sARB', target, s );

return
