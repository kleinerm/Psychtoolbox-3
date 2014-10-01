function glNamedFramebufferTextureLayerEXT( framebuffer, attachment, texture, level, layer )

% glNamedFramebufferTextureLayerEXT  Interface to OpenGL function glNamedFramebufferTextureLayerEXT
%
% usage:  glNamedFramebufferTextureLayerEXT( framebuffer, attachment, texture, level, layer )
%
% C function:  void glNamedFramebufferTextureLayerEXT(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferTextureLayerEXT', framebuffer, attachment, texture, level, layer );

return
