function params = glGetNamedFramebufferAttachmentParameterivEXT( framebuffer, attachment, pname )

% glGetNamedFramebufferAttachmentParameterivEXT  Interface to OpenGL function glGetNamedFramebufferAttachmentParameterivEXT
%
% usage:  params = glGetNamedFramebufferAttachmentParameterivEXT( framebuffer, attachment, pname )
%
% C function:  void glGetNamedFramebufferAttachmentParameterivEXT(GLuint framebuffer, GLenum attachment, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetNamedFramebufferAttachmentParameterivEXT', framebuffer, attachment, pname, params );

return
