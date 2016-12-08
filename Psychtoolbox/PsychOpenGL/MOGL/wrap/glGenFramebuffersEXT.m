function framebuffers = glGenFramebuffersEXT( n )

% glGenFramebuffersEXT  Interface to OpenGL function glGenFramebuffersEXT
%
% usage:  framebuffers = glGenFramebuffersEXT( n )
%
% C function:  void glGenFramebuffersEXT(GLsizei n, GLuint* framebuffers)

% 30-May-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

framebuffers = uint32(zeros(1,n+1));

moglcore( 'glGenFramebuffersEXT', n, framebuffers );
framebuffers = framebuffers(1:end-1);
return
% ---skip---
