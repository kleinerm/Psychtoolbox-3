function glDeleteOcclusionQueriesNV( n, ids )

% glDeleteOcclusionQueriesNV  Interface to OpenGL function glDeleteOcclusionQueriesNV
%
% usage:  glDeleteOcclusionQueriesNV( n, ids )
%
% C function:  void glDeleteOcclusionQueriesNV(GLsizei n, const GLuint* ids)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteOcclusionQueriesNV', n, uint32(ids) );

return
