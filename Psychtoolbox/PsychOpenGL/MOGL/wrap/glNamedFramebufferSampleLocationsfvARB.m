function glNamedFramebufferSampleLocationsfvARB( framebuffer, start, count, v )

% glNamedFramebufferSampleLocationsfvARB  Interface to OpenGL function glNamedFramebufferSampleLocationsfvARB
%
% usage:  glNamedFramebufferSampleLocationsfvARB( framebuffer, start, count, v )
%
% C function:  void glNamedFramebufferSampleLocationsfvARB(GLuint framebuffer, GLuint start, GLsizei count, const GLfloat* v)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferSampleLocationsfvARB', framebuffer, start, count, single(v) );

return
