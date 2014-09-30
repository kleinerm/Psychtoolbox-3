function glVertexAttribL4ui64vNV( index, v )

% glVertexAttribL4ui64vNV  Interface to OpenGL function glVertexAttribL4ui64vNV
%
% usage:  glVertexAttribL4ui64vNV( index, v )
%
% C function:  void glVertexAttribL4ui64vNV(GLuint index, const GLuint64EXT* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL4ui64vNV', index, uint64(v) );

return
