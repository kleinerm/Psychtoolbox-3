function glMultiTexCoord2d( target, s, t )

% glMultiTexCoord2d  Interface to OpenGL function glMultiTexCoord2d
%
% usage:  glMultiTexCoord2d( target, s, t )
%
% C function:  void glMultiTexCoord2d(GLenum target, GLdouble s, GLdouble t)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord2d', target, s, t );

return
