function glNamedFramebufferTextureFaceEXT( framebuffer, attachment, texture, level, face )

% glNamedFramebufferTextureFaceEXT  Interface to OpenGL function glNamedFramebufferTextureFaceEXT
%
% usage:  glNamedFramebufferTextureFaceEXT( framebuffer, attachment, texture, level, face )
%
% C function:  void glNamedFramebufferTextureFaceEXT(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLenum face)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferTextureFaceEXT', framebuffer, attachment, texture, level, face );

return
