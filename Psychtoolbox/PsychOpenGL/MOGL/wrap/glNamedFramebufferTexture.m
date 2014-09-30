function glNamedFramebufferTexture( framebuffer, attachment, texture, level )

% glNamedFramebufferTexture  Interface to OpenGL function glNamedFramebufferTexture
%
% usage:  glNamedFramebufferTexture( framebuffer, attachment, texture, level )
%
% C function:  void glNamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferTexture', framebuffer, attachment, texture, level );

return
