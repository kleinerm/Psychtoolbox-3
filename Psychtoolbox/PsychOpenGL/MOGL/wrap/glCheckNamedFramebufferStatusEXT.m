function r = glCheckNamedFramebufferStatusEXT( framebuffer, target )

% glCheckNamedFramebufferStatusEXT  Interface to OpenGL function glCheckNamedFramebufferStatusEXT
%
% usage:  r = glCheckNamedFramebufferStatusEXT( framebuffer, target )
%
% C function:  GLenum glCheckNamedFramebufferStatusEXT(GLuint framebuffer, GLenum target)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glCheckNamedFramebufferStatusEXT', framebuffer, target );

return
