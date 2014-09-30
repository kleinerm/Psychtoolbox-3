function pointer = glVertexArrayRangeAPPLE( length )

% glVertexArrayRangeAPPLE  Interface to OpenGL function glVertexArrayRangeAPPLE
%
% usage:  pointer = glVertexArrayRangeAPPLE( length )
%
% C function:  void glVertexArrayRangeAPPLE(GLsizei length, void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

pointer = (0);

moglcore( 'glVertexArrayRangeAPPLE', length, pointer );

return
