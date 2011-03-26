function glMap2f( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points )

% glMap2f  Interface to OpenGL function glMap2f
%
% usage:  glMap2f( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points )
%
% C function:  void glMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat* points)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=10,
    error('invalid number of arguments');
end

moglcore( 'glMap2f', target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, single(points) );

return
