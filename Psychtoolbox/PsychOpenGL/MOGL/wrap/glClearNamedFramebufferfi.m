function glClearNamedFramebufferfi( framebuffer, buffer, depth, stencil )

% glClearNamedFramebufferfi  Interface to OpenGL function glClearNamedFramebufferfi
%
% usage:  glClearNamedFramebufferfi( framebuffer, buffer, depth, stencil )
%
% C function:  void glClearNamedFramebufferfi(GLuint framebuffer, GLenum buffer, const GLfloat depth, GLint stencil)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glClearNamedFramebufferfi', framebuffer, buffer, depth, stencil );

return
