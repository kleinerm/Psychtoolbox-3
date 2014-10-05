function glTextureRenderbufferEXT( texture, target, renderbuffer )

% glTextureRenderbufferEXT  Interface to OpenGL function glTextureRenderbufferEXT
%
% usage:  glTextureRenderbufferEXT( texture, target, renderbuffer )
%
% C function:  void glTextureRenderbufferEXT(GLuint texture, GLenum target, GLuint renderbuffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTextureRenderbufferEXT', texture, target, renderbuffer );

return
