function glNamedFramebufferTexture2DEXT( framebuffer, attachment, textarget, texture, level )

% glNamedFramebufferTexture2DEXT  Interface to OpenGL function glNamedFramebufferTexture2DEXT
%
% usage:  glNamedFramebufferTexture2DEXT( framebuffer, attachment, textarget, texture, level )
%
% C function:  void glNamedFramebufferTexture2DEXT(GLuint framebuffer, GLenum attachment, GLenum textarget, GLuint texture, GLint level)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferTexture2DEXT', framebuffer, attachment, textarget, texture, level );

return
