function glFramebufferTexture1DEXT( target, attachment, textarget, texture, level )

% glFramebufferTexture1DEXT  Interface to OpenGL function glFramebufferTexture1DEXT
%
% usage:  glFramebufferTexture1DEXT( target, attachment, textarget, texture, level )
%
% C function:  void glFramebufferTexture1DEXT(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glFramebufferTexture1DEXT', target, attachment, textarget, texture, level );

return
