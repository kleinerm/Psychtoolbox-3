function glFramebufferTextureLayerEXT( target, attachment, texture, level, layer )

% glFramebufferTextureLayerEXT  Interface to OpenGL function glFramebufferTextureLayerEXT
%
% usage:  glFramebufferTextureLayerEXT( target, attachment, texture, level, layer )
%
% C function:  void glFramebufferTextureLayerEXT(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glFramebufferTextureLayerEXT', target, attachment, texture, level, layer );

return
