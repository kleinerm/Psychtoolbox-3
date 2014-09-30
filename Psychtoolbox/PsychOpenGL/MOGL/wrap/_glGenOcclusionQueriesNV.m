function ids = glGenOcclusionQueriesNV( n )

% glGenOcclusionQueriesNV  Interface to OpenGL function glGenOcclusionQueriesNV
%
% usage:  ids = glGenOcclusionQueriesNV( n )
%
% C function:  void glGenOcclusionQueriesNV(GLsizei n, GLuint* ids)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

ids = uint32(0);

moglcore( 'glGenOcclusionQueriesNV', n, ids );

return
