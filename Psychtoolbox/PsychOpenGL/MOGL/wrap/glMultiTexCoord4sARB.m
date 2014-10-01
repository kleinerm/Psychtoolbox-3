function glMultiTexCoord4sARB( target, s, t, r, q )

% glMultiTexCoord4sARB  Interface to OpenGL function glMultiTexCoord4sARB
%
% usage:  glMultiTexCoord4sARB( target, s, t, r, q )
%
% C function:  void glMultiTexCoord4sARB(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord4sARB', target, s, t, r, q );

return
