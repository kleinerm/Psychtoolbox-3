function glNamedFramebufferRenderbuffer( framebuffer, attachment, renderbuffertarget, renderbuffer )

% glNamedFramebufferRenderbuffer  Interface to OpenGL function glNamedFramebufferRenderbuffer
%
% usage:  glNamedFramebufferRenderbuffer( framebuffer, attachment, renderbuffertarget, renderbuffer )
%
% C function:  void glNamedFramebufferRenderbuffer(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferRenderbuffer', framebuffer, attachment, renderbuffertarget, renderbuffer );

return
