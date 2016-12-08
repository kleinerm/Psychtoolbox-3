function arrays = glGenVertexArrays( n )

% glGenVertexArrays  Interface to OpenGL function glGenVertexArrays
%
% usage:  arrays = glGenVertexArrays( n )
%
% C function:  void glGenVertexArrays(GLsizei n, GLuint* arrays)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

arrays = uint32(zeros(1,n+1));

moglcore( 'glGenVertexArrays', n, arrays );
arrays = arrays(1:end-1);
return
