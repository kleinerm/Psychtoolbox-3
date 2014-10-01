function glVertexArrayRangeNV( length, pointer )

% glVertexArrayRangeNV  Interface to OpenGL function glVertexArrayRangeNV
%
% usage:  glVertexArrayRangeNV( length, pointer )
%
% C function:  void glVertexArrayRangeNV(GLsizei length, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayRangeNV', length, pointer );

return
