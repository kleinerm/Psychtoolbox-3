function glNamedFramebufferSampleLocationsfvNV( framebuffer, start, count, v )

% glNamedFramebufferSampleLocationsfvNV  Interface to OpenGL function glNamedFramebufferSampleLocationsfvNV
%
% usage:  glNamedFramebufferSampleLocationsfvNV( framebuffer, start, count, v )
%
% C function:  void glNamedFramebufferSampleLocationsfvNV(GLuint framebuffer, GLuint start, GLsizei count, const GLfloat* v)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferSampleLocationsfvNV', framebuffer, start, count, single(v) );

return
