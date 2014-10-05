function glNamedFramebufferReadBuffer( framebuffer, src )

% glNamedFramebufferReadBuffer  Interface to OpenGL function glNamedFramebufferReadBuffer
%
% usage:  glNamedFramebufferReadBuffer( framebuffer, src )
%
% C function:  void glNamedFramebufferReadBuffer(GLuint framebuffer, GLenum src)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferReadBuffer', framebuffer, src );

return
