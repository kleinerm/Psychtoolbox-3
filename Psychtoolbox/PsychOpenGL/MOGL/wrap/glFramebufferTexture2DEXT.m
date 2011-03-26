function glFramebufferTexture2DEXT( target, attachment, textarget, texture, level )

% glFramebufferTexture2DEXT  Interface to OpenGL function glFramebufferTexture2DEXT
%
% usage:  glFramebufferTexture2DEXT( target, attachment, textarget, texture, level )
%
% C function:  void glFramebufferTexture2DEXT(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glFramebufferTexture2DEXT', target, attachment, textarget, texture, level );

return
