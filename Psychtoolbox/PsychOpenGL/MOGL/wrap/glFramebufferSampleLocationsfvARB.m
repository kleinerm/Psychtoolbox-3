function glFramebufferSampleLocationsfvARB( target, start, count, v )

% glFramebufferSampleLocationsfvARB  Interface to OpenGL function glFramebufferSampleLocationsfvARB
%
% usage:  glFramebufferSampleLocationsfvARB( target, start, count, v )
%
% C function:  void glFramebufferSampleLocationsfvARB(GLenum target, GLuint start, GLsizei count, const GLfloat* v)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glFramebufferSampleLocationsfvARB', target, start, count, single(v) );

return
