function glDeleteFramebuffersEXT( n, framebuffers )

% glDeleteFramebuffersEXT  Interface to OpenGL function glDeleteFramebuffersEXT
%
% usage:  glDeleteFramebuffersEXT( n, framebuffers )
%
% C function:  void glDeleteFramebuffersEXT(GLsizei n, const GLuint* framebuffers)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteFramebuffersEXT', n, uint32(framebuffers) );

return
