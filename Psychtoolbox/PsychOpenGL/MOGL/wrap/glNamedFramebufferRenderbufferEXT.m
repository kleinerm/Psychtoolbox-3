function glNamedFramebufferRenderbufferEXT( framebuffer, attachment, renderbuffertarget, renderbuffer )

% glNamedFramebufferRenderbufferEXT  Interface to OpenGL function glNamedFramebufferRenderbufferEXT
%
% usage:  glNamedFramebufferRenderbufferEXT( framebuffer, attachment, renderbuffertarget, renderbuffer )
%
% C function:  void glNamedFramebufferRenderbufferEXT(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferRenderbufferEXT', framebuffer, attachment, renderbuffertarget, renderbuffer );

return
