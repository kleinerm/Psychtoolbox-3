function glClearNamedFramebufferfi( framebuffer, buffer, drawbuffer, depth, stencil )

% glClearNamedFramebufferfi  Interface to OpenGL function glClearNamedFramebufferfi
%
% usage:  glClearNamedFramebufferfi( framebuffer, buffer, drawbuffer, depth, stencil )
%
% C function:  void glClearNamedFramebufferfi(GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);

% 25-Nov-2016 -- created (generated manually from bug-fixed OpenGL 4.5 spec)

% ---protected---
persistent onetimewarning
if isempty(onetimewarning)
  onetimewarning = 1;
  warning('Script called glClearNamedFramebufferfi(). Due to a past OpenGL-4.5 specification bug, this needs recent (bug-fixed) OpenGL vendor drivers or Mesa version >= 12.0, otherwise it could crash or malfunction.');
end

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glClearNamedFramebufferfi', framebuffer, buffer, drawbuffer, depth, stencil );

return
