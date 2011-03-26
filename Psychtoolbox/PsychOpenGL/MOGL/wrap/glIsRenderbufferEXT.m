function r = glIsRenderbufferEXT( renderbuffer )

% glIsRenderbufferEXT  Interface to OpenGL function glIsRenderbufferEXT
%
% usage:  r = glIsRenderbufferEXT( renderbuffer )
%
% C function:  GLboolean glIsRenderbufferEXT(GLuint renderbuffer)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsRenderbufferEXT', renderbuffer );

return
