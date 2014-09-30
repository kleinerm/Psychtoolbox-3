function params = glGetNamedFramebufferAttachmentParameteriv( framebuffer, attachment, pname )

% glGetNamedFramebufferAttachmentParameteriv  Interface to OpenGL function glGetNamedFramebufferAttachmentParameteriv
%
% usage:  params = glGetNamedFramebufferAttachmentParameteriv( framebuffer, attachment, pname )
%
% C function:  void glGetNamedFramebufferAttachmentParameteriv(GLuint framebuffer, GLenum attachment, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetNamedFramebufferAttachmentParameteriv', framebuffer, attachment, pname, params );

return
