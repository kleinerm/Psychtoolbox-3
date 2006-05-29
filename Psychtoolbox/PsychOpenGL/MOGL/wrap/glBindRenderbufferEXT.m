function glBindRenderbufferEXT( target, renderbuffer )

% glBindRenderbufferEXT  Interface to OpenGL function glBindRenderbufferEXT
%
% usage:  glBindRenderbufferEXT( target, renderbuffer )
%
% C function:  void glBindRenderbufferEXT(GLenum target, GLuint renderbuffer)

% 30-May-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBindRenderbufferEXT', target, renderbuffer );

return
