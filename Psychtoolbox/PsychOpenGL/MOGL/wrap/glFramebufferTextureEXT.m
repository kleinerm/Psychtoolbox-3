function glFramebufferTextureEXT( target, attachment, texture, level )

% glFramebufferTextureEXT  Interface to OpenGL function glFramebufferTextureEXT
%
% usage:  glFramebufferTextureEXT( target, attachment, texture, level )
%
% C function:  void glFramebufferTextureEXT(GLenum target, GLenum attachment, GLuint texture, GLint level)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glFramebufferTextureEXT', target, attachment, texture, level );

return
