function r = glIsFramebufferEXT( framebuffer )

% glIsFramebufferEXT  Interface to OpenGL function glIsFramebufferEXT
%
% usage:  r = glIsFramebufferEXT( framebuffer )
%
% C function:  GLboolean glIsFramebufferEXT(GLuint framebuffer)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsFramebufferEXT', framebuffer );

return
