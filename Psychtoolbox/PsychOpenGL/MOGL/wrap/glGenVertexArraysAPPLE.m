function arrays = glGenVertexArraysAPPLE( n )

% glGenVertexArraysAPPLE  Interface to OpenGL function glGenVertexArraysAPPLE
%
% usage:  arrays = glGenVertexArraysAPPLE( n )
%
% C function:  void glGenVertexArraysAPPLE(GLsizei n, GLuint* arrays)

% 30-Sep-2014 -- created (generated automatically from header files)
% 04-Jan-2015 -- Made working by manual allocation (MK)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

arrays = uint32(zeros(1,n));

moglcore( 'glGenVertexArraysAPPLE', n, arrays );

return
