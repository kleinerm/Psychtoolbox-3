function arrays = glCreateVertexArrays( n )

% glCreateVertexArrays  Interface to OpenGL function glCreateVertexArrays
%
% usage:  arrays = glCreateVertexArrays( n )
%
% C function:  void glCreateVertexArrays(GLsizei n, GLuint* arrays)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

arrays = uint32(0);

moglcore( 'glCreateVertexArrays', n, arrays );

return
