function glNamedFramebufferTexture1DEXT( framebuffer, attachment, textarget, texture, level )

% glNamedFramebufferTexture1DEXT  Interface to OpenGL function glNamedFramebufferTexture1DEXT
%
% usage:  glNamedFramebufferTexture1DEXT( framebuffer, attachment, textarget, texture, level )
%
% C function:  void glNamedFramebufferTexture1DEXT(GLuint framebuffer, GLenum attachment, GLenum textarget, GLuint texture, GLint level)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferTexture1DEXT', framebuffer, attachment, textarget, texture, level );

return
