function glMultiTexCoord3dARB( target, s, t, r )

% glMultiTexCoord3dARB  Interface to OpenGL function glMultiTexCoord3dARB
%
% usage:  glMultiTexCoord3dARB( target, s, t, r )
%
% C function:  void glMultiTexCoord3dARB(GLenum target, GLdouble s, GLdouble t, GLdouble r)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3dARB', target, s, t, r );

return
