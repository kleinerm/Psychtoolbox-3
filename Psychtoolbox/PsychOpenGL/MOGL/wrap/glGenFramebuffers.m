function framebuffers = glGenFramebuffers( n )

% glGenFramebuffers  Interface to OpenGL function glGenFramebuffers
%
% usage:  framebuffers = glGenFramebuffers( n )
%
% C function:  void glGenFramebuffers(GLsizei n, GLuint* framebuffers)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

framebuffers = uint32(zeros(1,n+1));

moglcore( 'glGenFramebuffers', n, framebuffers );
framebuffers = framebuffers(1:end-1);
return
