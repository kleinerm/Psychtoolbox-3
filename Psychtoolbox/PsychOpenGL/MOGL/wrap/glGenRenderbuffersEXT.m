function renderbuffers = glGenRenderbuffersEXT( n )

% glGenRenderbuffersEXT  Interface to OpenGL function glGenRenderbuffersEXT
%
% usage:  renderbuffers = glGenRenderbuffersEXT( n )
%
% C function:  void glGenRenderbuffersEXT(GLsizei n, GLuint* renderbuffers)

% 30-May-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

renderbuffers = uint32(zeros(1,n));

moglcore( 'glGenRenderbuffersEXT', n, renderbuffers );

return
% ---skip---
