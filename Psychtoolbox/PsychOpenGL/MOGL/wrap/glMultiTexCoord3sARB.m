function glMultiTexCoord3sARB( target, s, t, r )

% glMultiTexCoord3sARB  Interface to OpenGL function glMultiTexCoord3sARB
%
% usage:  glMultiTexCoord3sARB( target, s, t, r )
%
% C function:  void glMultiTexCoord3sARB(GLenum target, GLshort s, GLshort t, GLshort r)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3sARB', target, s, t, r );

return
