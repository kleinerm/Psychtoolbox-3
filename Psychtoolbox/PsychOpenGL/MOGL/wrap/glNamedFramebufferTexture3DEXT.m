function glNamedFramebufferTexture3DEXT( framebuffer, attachment, textarget, texture, level, zoffset )

% glNamedFramebufferTexture3DEXT  Interface to OpenGL function glNamedFramebufferTexture3DEXT
%
% usage:  glNamedFramebufferTexture3DEXT( framebuffer, attachment, textarget, texture, level, zoffset )
%
% C function:  void glNamedFramebufferTexture3DEXT(GLuint framebuffer, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferTexture3DEXT', framebuffer, attachment, textarget, texture, level, zoffset );

return
