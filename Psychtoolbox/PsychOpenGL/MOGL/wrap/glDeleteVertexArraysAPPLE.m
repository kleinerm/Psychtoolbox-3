function glDeleteVertexArraysAPPLE( n, arrays )

% glDeleteVertexArraysAPPLE  Interface to OpenGL function glDeleteVertexArraysAPPLE
%
% usage:  glDeleteVertexArraysAPPLE( n, arrays )
%
% C function:  void glDeleteVertexArraysAPPLE(GLsizei n, const GLuint* arrays)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteVertexArraysAPPLE', n, uint32(arrays) );

return
