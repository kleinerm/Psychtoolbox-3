function glNamedFramebufferTextureEXT( framebuffer, attachment, texture, level )

% glNamedFramebufferTextureEXT  Interface to OpenGL function glNamedFramebufferTextureEXT
%
% usage:  glNamedFramebufferTextureEXT( framebuffer, attachment, texture, level )
%
% C function:  void glNamedFramebufferTextureEXT(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferTextureEXT', framebuffer, attachment, texture, level );

return
