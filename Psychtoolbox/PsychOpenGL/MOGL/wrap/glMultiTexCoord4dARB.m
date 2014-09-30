function glMultiTexCoord4dARB( target, s, t, r, q )

% glMultiTexCoord4dARB  Interface to OpenGL function glMultiTexCoord4dARB
%
% usage:  glMultiTexCoord4dARB( target, s, t, r, q )
%
% C function:  void glMultiTexCoord4dARB(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord4dARB', target, s, t, r, q );

return
