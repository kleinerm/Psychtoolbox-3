function pointer = glFlushVertexArrayRangeAPPLE( length )

% glFlushVertexArrayRangeAPPLE  Interface to OpenGL function glFlushVertexArrayRangeAPPLE
%
% usage:  pointer = glFlushVertexArrayRangeAPPLE( length )
%
% C function:  void glFlushVertexArrayRangeAPPLE(GLsizei length, void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

pointer = (0);

moglcore( 'glFlushVertexArrayRangeAPPLE', length, pointer );

return
