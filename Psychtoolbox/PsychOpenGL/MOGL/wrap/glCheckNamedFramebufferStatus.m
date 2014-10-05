function r = glCheckNamedFramebufferStatus( framebuffer, target )

% glCheckNamedFramebufferStatus  Interface to OpenGL function glCheckNamedFramebufferStatus
%
% usage:  r = glCheckNamedFramebufferStatus( framebuffer, target )
%
% C function:  GLenum glCheckNamedFramebufferStatus(GLuint framebuffer, GLenum target)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glCheckNamedFramebufferStatus', framebuffer, target );

return
