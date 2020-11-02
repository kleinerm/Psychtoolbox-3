function glMulticastFramebufferSampleLocationsfvNV( gpu, framebuffer, start, count, v )

% glMulticastFramebufferSampleLocationsfvNV  Interface to OpenGL function glMulticastFramebufferSampleLocationsfvNV
%
% usage:  glMulticastFramebufferSampleLocationsfvNV( gpu, framebuffer, start, count, v )
%
% C function:  void glMulticastFramebufferSampleLocationsfvNV(GLuint gpu, GLuint framebuffer, GLuint start, GLsizei count, const GLfloat* v)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMulticastFramebufferSampleLocationsfvNV', gpu, framebuffer, start, count, single(v) );

return
