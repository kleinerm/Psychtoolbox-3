function glMultiTexCoord4d( target, s, t, r, q )

% glMultiTexCoord4d  Interface to OpenGL function glMultiTexCoord4d
%
% usage:  glMultiTexCoord4d( target, s, t, r, q )
%
% C function:  void glMultiTexCoord4d(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord4d', target, s, t, r, q );

return
