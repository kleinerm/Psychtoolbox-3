function glMultiTexCoord2dARB( target, s, t )

% glMultiTexCoord2dARB  Interface to OpenGL function glMultiTexCoord2dARB
%
% usage:  glMultiTexCoord2dARB( target, s, t )
%
% C function:  void glMultiTexCoord2dARB(GLenum target, GLdouble s, GLdouble t)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord2dARB', target, s, t );

return
