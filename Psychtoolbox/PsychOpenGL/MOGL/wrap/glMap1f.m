function glMap1f( target, u1, u2, stride, order, points )

% glMap1f  Interface to OpenGL function glMap1f
%
% usage:  glMap1f( target, u1, u2, stride, order, points )
%
% C function:  void glMap1f(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat* points)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glMap1f', target, u1, u2, stride, order, single(points) );

return
