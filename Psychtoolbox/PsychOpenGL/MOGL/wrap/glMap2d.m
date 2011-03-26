function glMap2d( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points )

% glMap2d  Interface to OpenGL function glMap2d
%
% usage:  glMap2d( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points )
%
% C function:  void glMap2d(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble* points)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=10,
    error('invalid number of arguments');
end

moglcore( 'glMap2d', target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, double(points) );

return
