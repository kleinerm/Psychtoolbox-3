function framebuffers = glCreateFramebuffers( n )

% glCreateFramebuffers  Interface to OpenGL function glCreateFramebuffers
%
% usage:  framebuffers = glCreateFramebuffers( n )
%
% C function:  void glCreateFramebuffers(GLsizei n, GLuint* framebuffers)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

framebuffers = uint32(0);

moglcore( 'glCreateFramebuffers', n, framebuffers );

return
