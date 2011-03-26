function glMap1d( target, u1, u2, stride, order, points )

% glMap1d  Interface to OpenGL function glMap1d
%
% usage:  glMap1d( target, u1, u2, stride, order, points )
%
% C function:  void glMap1d(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble* points)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glMap1d', target, u1, u2, stride, order, double(points) );

return
