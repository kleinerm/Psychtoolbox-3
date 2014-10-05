function glNamedFramebufferTextureLayer( framebuffer, attachment, texture, level, layer )

% glNamedFramebufferTextureLayer  Interface to OpenGL function glNamedFramebufferTextureLayer
%
% usage:  glNamedFramebufferTextureLayer( framebuffer, attachment, texture, level, layer )
%
% C function:  void glNamedFramebufferTextureLayer(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferTextureLayer', framebuffer, attachment, texture, level, layer );

return
